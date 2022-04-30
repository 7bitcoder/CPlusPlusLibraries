#include <chrono>
#include <condition_variable>
#include <format>
#include <functional>
#include <memory>
#include <mutex>
#include <ranges>
#include <setjmp.h>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vcruntime.h>

#include "MemoryManager.hpp"

#ifdef _WIN32
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <winuser.h>

#include <processthreadsapi.h>

#define WINDOWS
#endif

#ifdef linux

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define LINUX
#endif

namespace sd
{
    namespace
    {
        const std::thread::id MAIN_THREAD_ID = std::this_thread::get_id();

// #define __READ_RBP() __asm__ volatile("movq %%rbp, %0" : "=r"(__rbp))
#define __READ_RSP(rsp) __asm__ volatile("movq %%rsp, %0" : "=r"(rsp))

#ifdef WINDOWS

        auto getStackBounds()
        {
            intptr_t *rsp;
            __READ_RSP(rsp);
            ULONG_PTR lowLimit;
            ULONG_PTR highLimit;
            GetCurrentThreadStackLimits(&lowLimit, &highLimit);

            return std::make_tuple((uint8_t *)highLimit - 10, (uint8_t *)lowLimit, (uint8_t *)rsp);
        }

#endif
    } // namespace

    class MemoryManagerImpl final : public IMemoryManagerImpl
    {
      private:
        std::unordered_map<void *, ObjectMetadata> _objectsMetadata;

        size_t _memoryAllocated = 0;
        size_t _memoryLimit = 1 * 1024 * 1024; // ~1MB

      public:
        ~MemoryManagerImpl()
        {
            // cleanup if running in local threads
            if (std::this_thread::get_id() != MAIN_THREAD_ID)
            {
                clear();
            }
        }

        void clear()
        {
            for (auto &item : _objectsMetadata)
            {
                auto &[ptr, meta] = item;
                auto deleter = meta.typeInfo->deleter;
                (*deleter)(ptr); // (2)
            }
        }

        size_t garbageCollect() final
        {
            auto allocatedMemory = _memoryAllocated;
            mark();
            sweep();
            return allocatedMemory - _memoryAllocated;
        }

        void registerObject(void *object, const TypeInfo *typeInfo) final
        {
            _memoryAllocated += typeInfo->size;
            _objectsMetadata[object] = {.marked = false, .typeInfo = typeInfo};
            if (!isGBCollectionNeeded())
            {
                return;
            }
            auto freedBytes = garbageCollect();
            if (!freedBytes)
            {
                _memoryLimit *= 2;
            }
        }

      private:
        bool isGBCollectionNeeded() { return _memoryAllocated > _memoryLimit; }

        void mark()
        {
            auto worklist = getRoots();

            while (!worklist.empty())
            {
                auto o = worklist.back();
                worklist.pop_back();
                auto &meta = getObjectMetadata(o);

                if (meta.marked)
                {
                    continue;
                }
                meta.marked = true;
                for (const auto &p : getInnerObjects(o))
                {
                    worklist.push_back(p);
                }
            }
        }

        void sweep()
        {
            std::erase_if(_objectsMetadata, [this](auto &item) {
                auto &[ptr, meta] = item;
                if (meta.marked)
                {
                    meta.marked = false;
                    return false;
                }
                else
                {
                    auto deleter = meta.typeInfo->deleter;
                    (*deleter)(ptr); // (2)
                    _memoryAllocated -= meta.typeInfo->size;
                    return true;
                }
            });
        }

        ObjectMetadata &getObjectMetadata(void *object) { return _objectsMetadata.at(object); }

        std::vector<void *> getRoots()
        {

            // push local variables  stored in registers onto the stack.
            jmp_buf jb;
            setjmp(jb);

            auto [top, bot, rsp] = getStackBounds();

            std::vector<void *> result;
            while (rsp < top)
            {
                auto address = (void *)*reinterpret_cast<void **>(rsp);
                if (_objectsMetadata.contains(address))
                {
                    result.emplace_back(address);
                }
                rsp++;
            }

            return result;
        }

        std::vector<void *> getInnerObjects(void *object)
        {
            auto p = (uint8_t *)object;
            auto end = (p + getObjectMetadata(object).typeInfo->size);
            std::vector<void *> result;
            while (p < end)
            {
                auto address = (void *)*reinterpret_cast<void **>(p);
                if (_objectsMetadata.contains(address))
                {
                    result.emplace_back(address);
                }
                p++;
            }
            return result;
        }
    };

    MemoryManager &MemoryManager::instance()
    {
        static thread_local MemoryManager ob{std::make_unique<MemoryManagerImpl>()};
        return ob;
    }

    MemoryManager::MemoryManager(std::unique_ptr<IMemoryManagerImpl> impl) : _impl(std::move(impl)) {}

    size_t MemoryManager::garbageCollect() { return _impl->garbageCollect(); }
} // namespace sd