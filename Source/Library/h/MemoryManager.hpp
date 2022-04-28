#pragma once

#include <list>
#include <map>
#include <memory>
#include <string>

namespace sd
{
    struct Object
    {
        virtual std::string toString();
        virtual size_t getHashCode();
        virtual void finalize();
    };

    class MemoryManager
    {
      private:
        std::list<std::shared_ptr<Object>> _hooks;

        MemoryManager() = default;

      public:
        MemoryManager(const MemoryManager &) = delete;
        MemoryManager &operator=(const MemoryManager &) = delete;

        static MemoryManager &instance();

        template <class T, class... Args> std::shared_ptr<T> allocate(Args &&...args)
        {
            auto ptr = std::make_shared<T>(std::forward<Args>(args)...);
            _hooks.push_back(ptr);
            return ptr;
        }

        void garbageCollect();
    };
} // namespace sd