#pragma once

#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

namespace sd
{
    struct Object
    {
        virtual std::string toString();
        virtual size_t getHashCode();
    };

    struct IMemoryManager
    {
        virtual void runGCInBackground() = 0;
        virtual void garbageCollect() = 0;

        virtual ~IMemoryManager() {}
    };

    class MemoryManager : public IMemoryManager
    {
      private:
        std::mutex _mutex;
        std::jthread _runner;
        std::list<std::shared_ptr<Object>> _hooks;

        MemoryManager() = default;

      public:
        MemoryManager(const MemoryManager &) = delete;
        MemoryManager &operator=(const MemoryManager &) = delete;

        static MemoryManager &instance();

        template <class T, class... Args> std::shared_ptr<T> create(Args &&...args)
        {
            std::scoped_lock l{_mutex};
            auto ptr = std::make_shared<T>(std::forward<Args>(args)...);
            _hooks.push_back(ptr);
            return ptr;
        }

        void runGCInBackground() override;

        void garbageCollect() override;
    };
} // namespace sd