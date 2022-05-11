#include <setjmp.h>
#include <vector>

#include "DetectOs.hpp"
#include "MemoryManager.hpp"

#ifdef WINDOWS
#include <windows.h>

#include <processthreadsapi.h>
#endif

#if defined(LINUX) || defined(APPLE)
#include <pthread.h>
#endif

namespace sd
{
    namespace
    {

#define __READ_RBP(rbp) __asm__ volatile("movq %%rbp, %0" : "=r"(rbp)) // x86
#define __READ_RBP_ARM(rbp) __asm__ volatile("mov %0, sp" : "=r"(rbp)) // arm64
#define __READ_RSP(rsp) __asm__ volatile("movq %%rsp, %0" : "=r"(rsp))

        uint8_t *getStackRsp()
        {
            intptr_t *ptr;
#ifdef APPLE
            __READ_RBP_ARM(ptr);
#else
            __READ_RSP(ptr);
#endif
            return (uint8_t *)ptr;
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
#ifdef APPLE
        auto getStackBounds()
        {
            auto rsp = getStackRsp();
            pthread_attr_t attrs;
            auto self = pthread_self();
            void *stack_ptr = pthread_get_stackaddr_np(self);
            size_t stack_size = pthread_get_stacksize_np(self);

            return std::make_tuple((uint8_t *)stack_ptr - 10, (uint8_t *)stack_ptr - stack_size, rsp);
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
        _objectsRegister.forEach([this](IObjectHolder &objectHolder) { destroyObject(objectHolder); });
        _objectsRegister.clear();
    }

    void MemoryManager::destroyObject(IObjectHolder &objectHolder)
    {
        auto objectSize = objectHolder.getObjectSize();
        objectHolder.destroyObject();
        _allocatedMemory -= objectSize;
    }

    bool MemoryManager::isGBCollectionNeeded() { return getAllocatedMemory() > getMemoryLimit(); }

    void MemoryManager::mark()
    {
        auto worklist = getRoots();

        while (!worklist.empty())
        {
            auto ptr = worklist.back();
            worklist.pop_back();
            auto &objectHolder = _objectsRegister.getObjectHolder(ptr);

            if (objectHolder.isMarked())
            {
                continue;
            }
            objectHolder.mark();
            for (const auto &p : getInnerObjects(objectHolder))
            {
                worklist.push_back(p);
            }
        }
    }

    void MemoryManager::sweep()
    {
        _objectsRegister.unregisterIf([this](IObjectHolder &objectHolder) {
            if (objectHolder.isMarked())
            {
                objectHolder.unmark();
                return false;
            }
            else
            {
                destroyObject(objectHolder);
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
            if (_objectsRegister.isObjectRegistered(address))
            {
                result.emplace_back(address);
            }
            rsp++;
        }

        return result;
    }

    std::vector<void *> MemoryManager::getInnerObjects(const IObjectHolder &objectHolder)
    {
        auto p = (uint8_t *)objectHolder.getObjectPtr();
        auto end = (p + objectHolder.getObjectSize());
        std::vector<void *> result;
        while (p < end)
        {
            auto address = (void *)*reinterpret_cast<void **>(p);
            if (_objectsRegister.isObjectRegistered(address))
            {
                result.emplace_back(address);
            }
            p++;
        }
        return result;
    }
} // namespace sd