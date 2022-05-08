#pragma once

#include <algorithm>
#include <cstddef>
#include <memory>
#include <unordered_map>

namespace sd
{

    struct IMemoryManager
    {
        virtual size_t garbageCollect() = 0;
        virtual size_t getAllocatedMemory() const = 0;

        virtual ~IMemoryManager() {}
    };

    class MemoryManager : public IMemoryManager
    {
#pragma region HelperClasses
      private:
        struct IObject
        {
            virtual void *getPtr() const = 0;

            virtual size_t getSize() const = 0;

            virtual bool isMarked() const = 0;
            virtual void mark() = 0;
            virtual void unmark() = 0;

            virtual bool isValid() const = 0;

            virtual ~IObject() {}
        };

        template <class T> class Object final : public IObject
        {
          private:
            bool _marked = false;
            std::unique_ptr<T> _ptr;

            Object(T *ptr) : _ptr(ptr) {}

          public:
            Object(const Object &) = delete;
            Object &operator=(const Object &) = delete;

            template <class... Args> static std::unique_ptr<Object<T>> create(Args &&...params)
            {
                return std::unique_ptr<Object<T>>(new Object{new T{std::forward<Args>(params)...}});
            };

            T *getTypedPtr() const { return _ptr.get(); }
            void *getPtr() const final { return _ptr.get(); }

            size_t getSize() const final { return sizeof(T); }

            bool isMarked() const final { return _marked; }
            void mark() final { _marked = true; }
            void unmark() final { _marked = false; }

            bool isValid() const final { return !!getPtr(); }
            operator bool() const { return isValid(); }
        };

        class ObjectsRegister
        {
          private:
            std::unordered_map<void *, std::unique_ptr<IObject>> _objectsMap;

          public:
            void registerObject(std::unique_ptr<IObject> ob) { _objectsMap.insert({ob->getPtr(), std::move(ob)}); }
            bool isObjectRegistered(void *objectPtr) const { return _objectsMap.contains(objectPtr); }
            IObject &getObject(void *objectPtr) { return *_objectsMap.at(objectPtr); }

            void clear() { _objectsMap.clear(); }

            size_t size() const { return _objectsMap.size(); }
            bool empty() const { return _objectsMap.empty(); }

            template <class Fn> void unregisterIf(Fn func)
            {
                std::erase_if(_objectsMap, [&](auto &pair) -> bool { return func(*pair.second); });
            }
            template <class Fn> void forEach(Fn func)
            {
                std::for_each(_objectsMap.begin(), _objectsMap.end(), [&](auto &pair) { func(*pair.second); });
            }
        };
#pragma endregion
      private:
        ObjectsRegister _objectRegister;

        size_t _allocatedMemory = 0;
        size_t _memoryLimit = 1 * 1024 * 1024; // ~1MB

        MemoryManager() = default;

      public:
        ~MemoryManager();
        MemoryManager(const MemoryManager &) = delete;
        MemoryManager &operator=(const MemoryManager &) = delete;

        /**
         * Get Memory Manager singeleton instance, note that each thread has its own memory manager
         */
        static MemoryManager &instance();

        /**
         * Get Pointner to newly created menagable object,
         * if object wont be recheable in stack scope it will be collected
         */
        template <class T, class... Args> T *create(Args &&...params)
        {
            std::unique_ptr<Object<T>> object = Object<T>::create(std::forward<Args>(params)...);
            T *ptr = object->getTypedPtr();
            _allocatedMemory += object->getSize();
            _objectRegister.registerObject(std::move(object));
            if (isGBCollectionNeeded())
            {
                garbageCollect();
            }
            if (isGBCollectionNeeded())
            {
                bumpMemoryLimit();
            }
            return ptr;
        }

        /**
         * Runs manually garbage collection, returns bytes freed during this collection
         */
        size_t garbageCollect() override;

        /**
         * Get number of bytes currently allocated by memory manager.
         */
        size_t getAllocatedMemory() const override;

      private:
        void clear();

        bool isGBCollectionNeeded();
        void mark();
        void sweep();

        std::vector<void *> getRoots();
        std::vector<void *> getInnerObjects(const IObject &object);

        size_t getMemoryLimit() const;
        void bumpMemoryLimit();
    };

    /**
     * Equivalent of sd::MemoryManager::instance().create<T>(args...), but shorter
     */
    template <class T, class... Args> T *make(Args &&...params)
    {
        return MemoryManager::instance().create<T>(std::forward<Args>(params)...);
    }
} // namespace sd