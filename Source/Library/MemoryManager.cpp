#include <format>
#include <functional>
#include <ranges>

#include "MemoryManager.hpp"

namespace sd
{

    MemoryManager &MemoryManager::instance()
    {
        static MemoryManager ob;
        return ob;
    }

    void MemoryManager::garbageCollect()
    {
        _hooks.remove_if([](auto &hook) {
            auto remove = hook.use_count() == 1;
            if (remove)
            {
                hook->finalize();
            }
            return remove;
        });
    }

    std::string Object::toString() { return std::format("[Object: {:p}]", static_cast<void *>(this)); }
    size_t Object::getHashCode() { return reinterpret_cast<std::uintptr_t>(this); }
    void Object::finalize() {}
} // namespace sd