#pragma once
#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>

namespace sd
{

    struct ICacheItem
    {
        virtual const std::string &GetKey() const = 0;
        virtual const void *GetValue() const = 0;

        virtual ~ICacheItem(){};
    };

    template <class TValue> class CacheItem final : public ICacheItem
    {
      private:
        std::string _key;
        TValue _value;

      public:
        using Ptr = std::unique_ptr<CacheItem<TValue>>;

        static Ptr Make(const std::string &key, TValue &&value)
        {
            return Ptr(new CacheItem<TValue>(key, std::move(value)));
        }

        CacheItem(const std::string &key, TValue &&value) : _key(key), _value(value) {}

        const std::string &GetKey() const final { return _key; }
        const void *GetValue() const final { return &GetTypedValue(); }
        const TValue &GetTypedValue() const { return _value; }
    };

    struct ICachePolicy
    {
        virtual void CallOnRemove(const void *value) const = 0;
        virtual void CallOnUpdate(const void *oldValue, const void *newValue) const = 0;

        virtual ~ICachePolicy() {}
    };

    template <class TValue> class CachePolicy final : public ICachePolicy
    {
      public:
        using UpdateCallback = std::function<void(const TValue *, const TValue *)>;
        using RemoveCallback = std::function<void(const TValue *)>;

      private:
        UpdateCallback _updateCallback;
        RemoveCallback _removeCallback;

      public:
        std::unique_ptr<CachePolicy<TValue>> static Make(UpdateCallback updateCallback = nullptr,
                                                         RemoveCallback removeCallback = nullptr)
        {
            return std::unique_ptr<CachePolicy<TValue>>(new CachePolicy<TValue>(updateCallback, removeCallback));
        }

        CachePolicy(UpdateCallback updateCallback = nullptr, RemoveCallback removeCallback = nullptr)
            : _updateCallback(updateCallback), _removeCallback(removeCallback)
        {
        }

        void SetOnUpdateCallback(UpdateCallback updateCallback) { _updateCallback = updateCallback; }

        void SetOnRemoveCallback(RemoveCallback removeCallback) { _removeCallback = removeCallback; }

        void CallOnRemove(const void *value) const final
        {
            if (_removeCallback)
            {
                _removeCallback(static_cast<const TValue *>(value));
            }
        }

        void CallOnUpdate(const void *oldValue, const void *newValue) const final
        {
            if (_updateCallback)
            {
                _updateCallback(static_cast<const TValue *>(oldValue), static_cast<const TValue *>(newValue));
            }
        }
    };

    struct ICache
    {
        virtual bool Add(std::unique_ptr<ICacheItem> itemPtr, std::unique_ptr<ICachePolicy> policy = nullptr) = 0;

        virtual bool Set(std::unique_ptr<ICacheItem> itemPtr, std::unique_ptr<ICachePolicy> policy = nullptr) = 0;

        virtual const void *AddOrGetExisting(std::unique_ptr<ICacheItem> itemPtr,
                                             std::unique_ptr<ICachePolicy> policy = nullptr) = 0;

        virtual const void *Get(const std::string &key) const = 0;

        virtual bool Remove(const std::string &key) = 0;

        virtual bool Contains(const std::string &key) const = 0;

        virtual size_t Count() const = 0;

        virtual ~ICache() {}
    };

    class Cache final : public ICache
    {
      private:
        struct Data
        {
            std::unique_ptr<ICacheItem> item;
            std::unique_ptr<ICachePolicy> policy;
        };

        std::unordered_map<std::string, Data> _items;

      public:
        template <class TValue>
        bool Add(const std::string &key, TValue &&value, std::unique_ptr<CachePolicy<TValue>> policy = nullptr)
        {
            return Add(CacheItem<TValue>::Make(key, std::move(value)), std::move(policy));
        }

        bool Add(std::unique_ptr<ICacheItem> itemPtr, std::unique_ptr<ICachePolicy> policy = nullptr) final;

        template <class TValue>
        const TValue *AddOrGetExisting(const std::string &key, TValue &&value,
                                       std::unique_ptr<CachePolicy<TValue>> policy = nullptr)
        {
            return AddOrGetExisting(CacheItem<TValue>::Make(key, std::move(value)), std::move(policy));
        }

        template <class TValue>
        const TValue *AddOrGetExisting(std::unique_ptr<CacheItem<TValue>> itemPtr,
                                       std::unique_ptr<CachePolicy<TValue>> policy = nullptr)
        {
            return static_cast<const TValue *>(AddOrGetExisting(std::move(itemPtr), std::move(policy)));
        }

        const void *AddOrGetExisting(std::unique_ptr<ICacheItem> itemPtr,
                                     std::unique_ptr<ICachePolicy> policy = nullptr) final;

        template <class TValue>
        bool Set(const std::string &key, TValue &&value, std::unique_ptr<CachePolicy<TValue>> policy = nullptr)
        {
            return Set(CacheItem<TValue>::Make(key, std::move(value)), std::move(policy));
        }

        template <class TValue>
        bool Set(std::unique_ptr<CacheItem<TValue>> itemPtr, std::unique_ptr<CachePolicy<TValue>> policy = nullptr)
        {
            return Set(std::move(itemPtr), std::move(policy));
        }

        bool Set(std::unique_ptr<ICacheItem> itemPtr, std::unique_ptr<ICachePolicy> policy = nullptr) final;

        template <class TValue> const TValue *Get(const std::string &key) const
        {
            return static_cast<const TValue *>(Get(key));
        }

        const void *Get(const std::string &key) const final;

        bool Remove(const std::string &key) final;

        bool Contains(const std::string &key) const final;

        size_t Count() const final;

      private:
        bool AddData(Data data);

        Data *GetEditableData(const std::string &key);

        const Data *GetData(const std::string &key) const;

        std::optional<Cache::Data> RemoveData(const std::string &key);

        bool ContainsData(const std::string &key) const;

        size_t CountData() const;
    };
} // namespace sd