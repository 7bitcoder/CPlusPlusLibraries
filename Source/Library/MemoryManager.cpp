#include <setjmp.h>

#include "DetectOs.hpp"
#include "MemoryManager.hpp"

#ifdef WINDOWS
#include <windows.h>

#include <processthreadsapi.h>
#endif

#ifdef LINUX
#include <pthread.h>
#endif

namespace sd
{
    namespace
    {

#define __READ_RBP(rbp) __asm__ volatile("movq %%rbp, %0" : "=r"(rbp))
#define __READ_RSP(rsp) __asm__ volatile("movq %%rsp, %0" : "=r"(rsp))

        auto getStackRsp()
        {
            intptr_t *rsp;
            __READ_RSP(rsp);
            return (uint8_t *)rsp;
        }

#ifdef WINDOWS
        auto getStackBounds()
        {
            auto rsp = getStackRsp();
            ULONG_PTR lowLimit, highLimit;
            GetCurrentThreadStackLimits(&lowLimit, &highLimit);

            return std::make_tuple((uint8_t *)highLimit - 10, (uint8_t *)lowLimit, rsp);
        }
#endif
#ifdef LINUX
        auto getStackBounds()
        {
            auto rsp = getStackRsp();
            pthread_attr_t attrs;
            pthread_getattr_np(pthread_self(), &attrs);
            void *stack_ptr;
            size_t stack_size;
            pthread_attr_getstack(&attrs, &stack_ptr, &stack_size);

            return std::make_tuple((uint8_t *)stack_ptr + stack_size - 10, (uint8_t *)stack_ptr, rsp);
        }
#endif
    } // namespace

    MemoryManager &MemoryManager::instance()
    {
        static thread_local MemoryManager ob;
        return ob;
    }

    MemoryManager::~MemoryManager() { clear(); }

    size_t MemoryManager::garbageCollect()
    {
        auto snapshot = getAllocatedMemory();
        mark();
        sweep();
        auto freedBytes = snapshot - getAllocatedMemory();
        return freedBytes;
    }

    size_t MemoryManager::getAllocatedMemory() const { return _allocatedMemory; }

    size_t MemoryManager::getMemoryLimit() const { return _memoryLimit; }

    void MemoryManager::bumpMemoryLimit() { _memoryLimit *= 2; }

    void MemoryManager::clear()
    {
        _objectRegister.forEach([this](IObject &object) { _allocatedMemory -= object.getSize(); });
        _objectRegister.clear();
    }

    bool MemoryManager::isGBCollectionNeeded() { return getAllocatedMemory() > getMemoryLimit(); }

    void MemoryManager::mark()
    {
        auto worklist = getRoots();

        while (!worklist.empty())
        {
            auto ptr = worklist.back();
            worklist.pop_back();
            auto &object = _objectRegister.getObject(ptr);

            if (object.isMarked())
            {
                continue;
            }
            object.mark();
            for (const auto &p : getInnerObjects(object))
            {
                worklist.push_back(p);
            }
        }
    }

    void MemoryManager::sweep()
    {
        _objectRegister.unregisterIf([this](IObject &object) {
            if (object.isMarked())
            {
                object.unmark();
                return false;
            }
            else
            {
                _allocatedMemory -= object.getSize();
                return true;
            }
        });
    }

    std::vector<void *> MemoryManager::getRoots()
    {
        // push local variables  stored in registers onto the stack.
        jmp_buf jb;
        setjmp(jb);

        auto [top, bot, rsp] = getStackBounds();

        std::vector<void *> result;
        while (rsp < top)
        {
            auto address = (void *)*reinterpret_cast<void **>(rsp);
            if (_objectRegister.isObjectRegistered(address))
            {
                result.emplace_back(address);
            }
            rsp++;
        }

        return result;
    }

    std::vector<void *> MemoryManager::getInnerObjects(const IObject &object)
    {
        auto p = (uint8_t *)object.getPtr();
        auto end = (p + object.getSize());
        std::vector<void *> result;
        while (p < end)
        {
            auto address = (void *)*reinterpret_cast<void **>(p);
            if (_objectRegister.isObjectRegistered(address))
            {
                result.emplace_back(address);
            }
            p++;
        }
        return result;
    }
} // namespace sd