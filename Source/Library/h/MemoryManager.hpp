#pragma once

#include "MemoryManager.hpp"
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vcruntime.h>

namespace sd
{
    struct TypeInfo
    {
        size_t size = 0;
        void (*deleter)(void *) = nullptr;

        TypeInfo(size_t size, void (*deleter)(void *)) : size(size), deleter(deleter) {}
    };

    template <class T> struct Type
    {
        static const TypeInfo *getInfo()
        {
            static TypeInfo *info = new TypeInfo{sizeof(T), &del};
            return info;
        };

      private:
        static void del(void *object)
        {
            auto ptr = reinterpret_cast<T *>(object);
            delete ptr;
        }
    };

    struct ObjectMetadata
    {
        bool marked;
        const TypeInfo *typeInfo;
    };

    struct IMemoryManager
    {
        virtual void runGCInBackground() = 0;
        virtual void garbageCollect() = 0;

        virtual ~IMemoryManager() {}
    };

    struct IMemoryManagerImpl : public IMemoryManager
    {
        virtual void registerObject(void *object, const TypeInfo *typeInfo) = 0;
    };

    class MemoryManager : public IMemoryManager
    {
      private:
        std::unique_ptr<IMemoryManagerImpl> _impl;

        MemoryManager(std::unique_ptr<IMemoryManagerImpl>);

      public:
        MemoryManager(const MemoryManager &) = delete;
        MemoryManager &operator=(const MemoryManager &) = delete;

        static MemoryManager &instance();

        template <class T, class... Args> T *create(Args &&...params)
        {
            auto typeInfo = Type<T>::getInfo();
            auto ptr = new T(std::forward<Args>(params)...);
            _impl->registerObject(ptr, typeInfo);
            return ptr;
        }

        void runGCInBackground() override;

        void garbageCollect() override;
    };

    template <class T, class... Args> T *make(Args &&...params)
    {
        return MemoryManager::instance().create<T>(std::forward<Args>(params)...);
    }
} // namespace sd