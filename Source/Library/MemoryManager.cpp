#include <setjmp.h>

#include "MemoryManager.hpp"
#include "DetectOs.hpp"

#ifdef WINDOWS

#include <windows.h>

#include <processthreadsapi.h>

#endif

#ifdef LINUX
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <pthread.h>

#endif

namespace sd
{
    namespace
    {

#define __READ_RBP(rbp) __asm__ volatile("movq %%rbp, %0" : "=r"(rbp))
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
#ifdef LINUX

        auto getStackBounds()
        {
            intptr_t *rsp;
            __READ_RSP(rsp);
            pthread_attr_t attrs;
            pthread_getattr_np(pthread_self(), &attrs);
            void* stack_ptr;
            size_t stack_size;
            pthread_attr_getstack(&attrs, &stack_ptr, &stack_size);
            return std::make_tuple((uint8_t *)stack_ptr + stack_size - 10, (uint8_t *)rsp, (uint8_t *)rsp);
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
        _register.forEach([this](auto &object) { destroy(object); });
        _register.clear();
    }

    void MemoryManager::destroy(Object &object)
    {
        auto size = object.getSize();
        object.destroy();
        _allocatedMemory -= size;
    }

    bool MemoryManager::isGBCollectionNeeded() { return getAllocatedMemory() > getMemoryLimit(); }

    void MemoryManager::mark()
    {
        auto worklist = getRoots();

        while (!worklist.empty())
        {
            auto ptr = worklist.back();
            worklist.pop_back();
            auto &object = _register.getObject(ptr);

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
        _register.unregisterIf([this](auto &object) {
            if (object.isMarked())
            {
                object.unmark();
                return false;
            }
            else
            {
                destroy(object);
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
            if (_register.contains(address))
            {
                result.emplace_back(address);
            }
            rsp++;
        }

        return result;
    }

    std::vector<void *> MemoryManager::getInnerObjects(const Object &object)
    {
        auto p = (uint8_t *)object.getRawPtr();
        auto end = (p + object.getSize());
        std::vector<void *> result;
        while (p < end)
        {
            auto address = (void *)*reinterpret_cast<void **>(p);
            if (_register.contains(address))
            {
                result.emplace_back(address);
            }
            p++;
        }
        return result;
    }
} // namespace sd