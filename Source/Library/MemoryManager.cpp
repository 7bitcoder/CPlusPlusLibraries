#include <chrono>
#include <condition_variable>
#include <format>
#include <functional>
#include <ranges>

#include "MemoryManager.hpp"

namespace sd
{
    std::string Object::toString() { return std::format("[Object: {:p}]", static_cast<void *>(this)); }
    size_t Object::getHashCode() { return reinterpret_cast<std::uintptr_t>(this); }

    MemoryManager &MemoryManager::instance()
    {
        static MemoryManager ob;
        return ob;
    }

    void MemoryManager::runGCInBackground()
    {
        using namespace std::chrono_literals;
        _runner = std::jthread{[this](std::stop_token stoken) {
            while (!stoken.stop_requested())
            {
                std::mutex mutex_;
                std::unique_lock<std::mutex> ul_{mutex_};
                std::condition_variable_any{}.wait_for(ul_, 10s, [&stoken]() { return stoken.stop_requested(); });
                garbageCollect();
            }
        }};
    }

    void MemoryManager::garbageCollect()
    {
        std::scoped_lock l{_mutex};
        _hooks.remove_if([](auto &hook) { return hook.use_count() == 1; });
        // detect circles
    }

} // namespace sd