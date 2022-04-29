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
    } // namespace

    class MemoryManagerImpl final : public IMemoryManagerImpl
    {
      private:
        std::unordered_map<void *, ObjectMetadata> _objectsMetadata;

        std::jthread _runner;
        std::mutex _mutex;
        std::condition_variable_any _GCWaiter;

        size_t memoryAllocated = 0;
        intptr_t *_stackBegin = nullptr;

      public:
        MemoryManagerImpl()
        { // `main` frame pointer:
            __READ_RBP();
            _stackBegin = (intptr_t *)*__rbp;
        }

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
            std::lock_guard l{_mutex};
            mark();
            sweep();
        }

        void registerObject(void *object, const TypeInfo *typeInfo) final
        {
            {
                std::lock_guard l{_mutex};
                memoryAllocated += typeInfo->size;
                _objectsMetadata[object] = {.marked = false, .typeInfo = typeInfo};
            }
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
            std::vector<void *> result;

            // Some local variables (roots) can be stored in registers.
            // Use `setjmp` to push them all onto the stack.
            jmp_buf jb;
            setjmp(jb);

            __READ_RSP();
            auto rsp = (uint8_t *)__rsp;
            auto top = (uint8_t *)_stackBegin;

            while (rsp < top)
            {
                auto address = (void *)rsp;
                if (_objectsMetadata.count(address) != 0)
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
                auto address = (void *)p;
                if (_objectsMetadata.count(address) != 0)
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
        static MemoryManager ob{std::make_unique<MemoryManagerImpl>()};
        return ob;
    }

    MemoryManager::MemoryManager(std::unique_ptr<IMemoryManagerImpl> impl) : _impl(std::move(impl)) {}

    void MemoryManager::runGCInBackground() { _impl->runGCInBackground(); }

    void MemoryManager::garbageCollect() { _impl->garbageCollect(); }

} // namespace sd