#include <chrono>
#include <condition_variable>
#include <format>
#include <functional>
#include <memory>
#include <mutex>
#include <ranges>
#include <setjmp.h>
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
        /**
         * Frame pointer.
         */
        intptr_t *__rbp;

        /**
         * Stack pointer.
         */
        intptr_t *__rsp;

#define __READ_RBP() __asm__ volatile("movq %%rbp, %0" : "=r"(__rbp))
#define __READ_RSP() __asm__ volatile("movq %%rsp, %0" : "=r"(__rsp))

#ifdef WINDOWS
        NT_TIB *getTIB()
        {
#ifdef _M_IX86
            return (NT_TIB *)__readfsdword(0x18);
#elif _M_AMD64
            return (NT_TIB *)__readgsqword(0x30);
#else
#error unsupported architecture
#endif
        }

        std::pair<uint8_t *, uint8_t *> getStackBounds()
        {
            __READ_RSP();
            PULONG_PTR LowLimit;
            PULONG_PTR HighLimit;
            GetCurrentThreadStackLimits(LowLimit, HighLimit);
            NT_TIB *tib = getTIB();
            return {(uint8_t *)tib->StackBase, (uint8_t *)__rsp};
        }
#endif

    } // namespace

    class MemoryManagerImpl final : public IMemoryManagerImpl
    {
      private:
        std::unordered_map<void *, ObjectMetadata> _objectsMetadata;

        std::jthread _runner;
        std::mutex _mutex;
        std::condition_variable_any _GCWaiter;

        size_t memoryAllocated = 0;
        void *_stackBegin = nullptr;

      public:
        MemoryManagerImpl(void *stackBegin) : _stackBegin(stackBegin) {}

        void runGCInBackground() final
        {
            using namespace std::chrono_literals;
            _runner = std::jthread{[this](std::stop_token stoken) {
                while (!stoken.stop_requested())
                {
                    std::unique_lock<std::mutex> ul_{_mutex};
                    _GCWaiter.wait(ul_, []() { return true; });
                    garbageCollect();
                }
            }};
        }

        void garbageCollect() final
        {
            mark();
            sweep();
        }

        void registerObject(void *object, const TypeInfo *typeInfo) final
        {
            memoryAllocated += typeInfo->size;
            _objectsMetadata[object] = {.marked = false, .typeInfo = typeInfo};
            if (isGBCollectionNeeded())
            {
                //     _GCWaiter.notify_all();
                //     std::lock_guard l{_mutex};
                garbageCollect();
            }
        }

      private:
        bool isGBCollectionNeeded() { return memoryAllocated > 2000; }

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
                    memoryAllocated -= meta.typeInfo->size;
                    return true;
                }
            });
        }

        ObjectMetadata &getObjectMetadata(void *object) { return _objectsMetadata.at(object); }

        std::vector<void *> getRoots()
        {

            // Some local variables (roots) can be stored in registers.
            // Use `setjmp` to push them all onto the stack.
            jmp_buf jb;
            setjmp(jb);

            // auto [top, rsp] = getStackBounds();

            __READ_RSP();
            auto rsp = (uint8_t *)__rsp;
            auto top = (uint8_t *)_stackBegin;

            std::vector<void *> result;
            while (rsp < top)
            {
                try
                {
                    auto address = (void *)*(uintptr_t *)rsp;
                    if (_objectsMetadata.contains(address))
                    {
                        result.emplace_back(address);
                    }
                }
                catch (...)
                {
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
                auto address = (void *)*(uintptr_t *)p;
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
        static MemoryManager ob{std::make_unique<MemoryManagerImpl>(__builtin_frame_address(4))};
        return ob;
    }

    MemoryManager::MemoryManager(std::unique_ptr<IMemoryManagerImpl> impl) : _impl(std::move(impl)) {}

    void MemoryManager::runGCInBackground() { _impl->runGCInBackground(); }

    void MemoryManager::garbageCollect() { _impl->garbageCollect(); }

} // namespace sd