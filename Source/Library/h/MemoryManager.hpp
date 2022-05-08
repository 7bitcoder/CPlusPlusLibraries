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
        struct IObjectHolder
        {
            virtual void *getObjectPtr() const = 0;

            virtual size_t getObjectSize() const = 0;

            virtual bool isMarked() const = 0;
            virtual void mark() = 0;
            virtual void unmark() = 0;

            virtual void destroyObject() = 0;
            virtual bool isValid() const = 0;

            virtual ~IObjectHolder() {}
        };

        template <class T> class ObjectHolder final : public IObjectHolder
        {
          private:
            bool _marked = false;
            std::unique_ptr<T> _objectPtr;

            ObjectHolder(T *objectPtr) : _objectPtr(objectPtr) {}

          public:
            ObjectHolder(const ObjectHolder &) = delete;
            ObjectHolder &operator=(const ObjectHolder &) = delete;

            template <class... Args> static std::unique_ptr<ObjectHolder<T>> create(Args &&...params)
            {
                auto objectPtr = new T{std::forward<Args>(params)...};
                return std::unique_ptr<ObjectHolder<T>>(new ObjectHolder{objectPtr});
            };

            T *getTypedObjectPtr() const { return _objectPtr.get(); }
            void *getObjectPtr() const final { return _objectPtr.get(); }

            size_t getObjectSize() const final { return sizeof(T); }

            bool isMarked() const final { return _marked; }
            void mark() final { _marked = true; }
            void unmark() final { _marked = false; }

            void destroyObject() { _objectPtr.reset(); }
            bool isValid() const final { return !!getObjectPtr(); }
            operator bool() const { return isValid(); }
        };

        class ObjectsRegister
        {
          private:
            std::unordered_map<void *, std::unique_ptr<IObjectHolder>> _objectsMap;

          public:
            void registerObject(std::unique_ptr<IObjectHolder> objectHolder)
            {
                _objectsMap.insert({objectHolder->getObjectPtr(), std::move(objectHolder)});
            }
            bool isObjectRegistered(void *objectPtr) const { return _objectsMap.contains(objectPtr); }
            IObjectHolder &getObjectHolder(void *objectPtr) { return *_objectsMap.at(objectPtr); }

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
        ObjectsRegister _objectsRegister;

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
        template <class T, class... Args> T *createObject(Args &&...params)
        {
            std::unique_ptr<ObjectHolder<T>> objectHolderPtr = ObjectHolder<T>::create(std::forward<Args>(params)...);
            T *ptr = objectHolderPtr->getTypedObjectPtr();
            _allocatedMemory += objectHolderPtr->getObjectSize();
            _objectsRegister.registerObject(std::move(objectHolderPtr));
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
        void destroyObject(IObjectHolder &objectHolder);
        void clear();

        bool isGBCollectionNeeded();
        void mark();
        void sweep();

        std::vector<void *> getRoots();
        std::vector<void *> getInnerObjects(const IObjectHolder &objectHolder);

        size_t getMemoryLimit() const;
        void bumpMemoryLimit();
    };

    /**
     * Equivalent of sd::MemoryManager::instance().create<T>(args...), but shorter
     */
    template <class T, class... Args> T *make(Args &&...params)
    {
        return MemoryManager::instance().createObject<T>(std::forward<Args>(params)...);
    }
} // namespace sd