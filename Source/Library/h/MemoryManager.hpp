#pragma once

#include "MemoryManager.hpp"
#include <algorithm>
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
      private:
#pragma region HelperClasses
        class Object
        {
          public:
            class TypeInfo
            {
              private:
                using Deleter = void (*)(void *);

                size_t _size = 0;
                Deleter _deleter = nullptr;

              public:
                TypeInfo(size_t size, Deleter deleter) : _size(size), _deleter(deleter) {}

                size_t getSize() const { return _size; }
                Deleter getDeleter() const { return _deleter; }
            };

            template <class T> struct Type
            {
                template <class... A> static T *create(A &&...args) { return new T(std::forward<A>(args)...); };

                static void destroy(void *objectPtr) { delete reinterpret_cast<T *>(objectPtr); }

                static TypeInfo *getInfo()
                {
                    static TypeInfo *info = new TypeInfo{sizeof(T), &destroy};
                    return info;
                }
            };

            class Metadata
            {
              private:
                bool _marked = false;
                const TypeInfo *_typeInfo = nullptr;

              public:
                Metadata(TypeInfo *typeInfo) : _marked(false), _typeInfo(typeInfo) {}
                void mark() { _marked = true; }
                void unmark() { _marked = false; }
                bool isMarked() const { return _marked; }
                const TypeInfo *getTypeInfo() const { return _typeInfo; }
            };

          private:
            void *_ptr = nullptr;
            Metadata _metadata;

            Object(void *ptr, Metadata metadata) : _ptr(ptr), _metadata(metadata) {}

          public:
            Object(const Object &) = default;
            Object &operator=(const Object &) = default;

            template <class T, class... Args> static Object create(Args &&...params)
            {
                return Object{Type<T>::create(std::forward<Args>(params)...), {Type<T>::getInfo()}};
            };

            void *getRawPtr() const { return _ptr; }
            Metadata &getMetadata() { return _metadata; }
            const Metadata &getMetadata() const { return _metadata; }
            const TypeInfo *getTypeInfo() const { return getMetadata().getTypeInfo(); }

            size_t getSize() const { return getTypeInfo()->getSize(); }
            bool isMarked() const { return getMetadata().isMarked(); }
            void mark() { getMetadata().mark(); }
            void unmark() { getMetadata().unmark(); }

            void destroy() { (*getTypeInfo()->getDeleter())(getRawPtr()); }

          private:
        };

        class ObjectsRegister
        {
          private:
            std::unordered_map<void *, Object> _objectsMap;

          public:
            void registerNew(const Object &object) { _objectsMap.insert({object.getRawPtr(), object}); }
            bool contains(void *objectPtr) const { return _objectsMap.contains(objectPtr); }
            Object &getObject(void *objectPtr) { return _objectsMap.at(objectPtr); }

            void clear() { _objectsMap.clear(); }

            size_t numberOfRegisteredObjects() const { return _objectsMap.size(); }
            bool anyObjectRegistered() const { return _objectsMap.empty(); }

            template <class Fn> void unregisterIf(Fn predicate)
            {
                std::erase_if(_objectsMap, [&](auto &pair) { return predicate(pair.second); });
            }
            template <class Fn> void forEach(Fn predicate)
            {
                std::for_each(_objectsMap.begin(), _objectsMap.end(),
                              [&](auto &pair) { return predicate(pair.second); });
            }
        };
#pragma endregion

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
            auto object = Object::create<T>(std::forward<Args>(params)...);
            _allocatedMemory += object.getSize();
            _objectRegister.registerNew(object);
            if (isGBCollectionNeeded())
            {
                garbageCollect();
            }
            if (isGBCollectionNeeded())
            {
                bumpMemoryLimit();
            }
            return reinterpret_cast<T *>(object.getRawPtr());
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
        void destroy(Object &object);
        void clear();

        bool isGBCollectionNeeded();
        void mark();
        void sweep();

        std::vector<void *> getRoots();
        std::vector<void *> getInnerObjects(const Object &object);

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