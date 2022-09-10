#pragma once
#include <cstddef>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>

namespace sd
{

    template <class TValue> class CacheItem;
    struct CacheItemBase
    {
        using Ptr = std::unique_ptr<CacheItemBase>;

        virtual const std::string &GetKey() const = 0;
        virtual const void *GetValue() const = 0;

        template <class TValue> const CacheItem<TValue> *Upcast() const
        {
            return dynamic_cast<const CacheItem<TValue> *>(this);
        }

        template <class TValue> const TValue *GetValueAs() const
        {
            if (auto casted = Upcast<TValue>())
            {
                return casted->GetTypedValue();
            }
            return nullptr;
        }

        virtual ~CacheItemBase(){};
    };

    template <class TValue> class CacheItem final : public CacheItemBase
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
        const void *GetValue() const final { return GetTypedValue(); }
        const TValue *GetTypedValue() const { return &_value; }
    };

    struct ICachePolicy
    {
        using Ptr = std::unique_ptr<ICachePolicy>;

        virtual void CallOnRemove(const CacheItemBase *value) const = 0;
        virtual void CallOnUpdate(const CacheItemBase *oldValue, const CacheItemBase *newValue) const = 0;

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
        using Ptr = std::unique_ptr<CachePolicy<TValue>>;

        Ptr static Make(UpdateCallback updateCallback = nullptr, RemoveCallback removeCallback = nullptr)
        {
            return Ptr(new CachePolicy<TValue>(updateCallback, removeCallback));
        }

        CachePolicy(UpdateCallback updateCallback = nullptr, RemoveCallback removeCallback = nullptr)
            : _updateCallback(updateCallback), _removeCallback(removeCallback)
        {
        }

        void SetOnUpdateCallback(UpdateCallback updateCallback) { _updateCallback = updateCallback; }

        void SetOnRemoveCallback(RemoveCallback removeCallback) { _removeCallback = removeCallback; }

        void CallOnRemove(const CacheItemBase *item) const final
        {
            if (_removeCallback && item)
            {
                _removeCallback(item->GetValueAs<TValue>());
            }
        }

        void CallOnUpdate(const CacheItemBase *oldItem, const CacheItemBase *newItem) const final
        {
            if (_updateCallback && oldItem && newItem)
            {
                _updateCallback(oldItem->GetValueAs<TValue>(), newItem->GetValueAs<TValue>());
            }
        }
    };

    struct ICache
    {
        virtual bool Add(CacheItemBase::Ptr itemPtr, ICachePolicy::Ptr policy = nullptr) = 0;

        virtual bool Set(CacheItemBase::Ptr itemPtr, ICachePolicy::Ptr policy = nullptr) = 0;

        virtual const void *Get(const std::string &key) const = 0;

        virtual const CacheItemBase *GetItem(const std::string &key) const = 0;

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
            CacheItemBase::Ptr item;
            ICachePolicy::Ptr policy;
        };

        std::unordered_map<std::string, Data> _items;

      public:
        Cache() = default;
        Cache(const Cache &) = delete;
        Cache(Cache &&) = delete;

        Cache &operator=(const Cache &) = delete;
        Cache &operator=(Cache &&) = delete;

        template <class TValue>
        bool Add(const std::string &key, TValue &&value, typename CachePolicy<TValue>::Ptr policy = nullptr)
        {
            return Add<TValue>(CacheItem<TValue>::Make(key, std::move(value)), std::move(policy));
        }

        template <class TValue>
        bool Add(typename CacheItem<TValue>::Ptr itemPtr, typename CachePolicy<TValue>::Ptr policy = nullptr)
        {
            CacheItemBase::Ptr itemPtrCasted = std::move(itemPtr);
            return Add(std::move(itemPtrCasted), std::move(policy));
        }

        bool Add(CacheItemBase::Ptr itemPtr, ICachePolicy::Ptr policy = nullptr) final;

        template <class TValue>
        bool Set(const std::string &key, TValue &&value, typename CachePolicy<TValue>::Ptr policy = nullptr)
        {
            return Set<TValue>(CacheItem<TValue>::Make(key, std::move(value)), std::move(policy));
        }

        template <class TValue>
        bool Set(typename CacheItem<TValue>::Ptr itemPtr, typename CachePolicy<TValue>::Ptr policy = nullptr)
        {
            CacheItemBase::Ptr itemPtrCasted = std::move(itemPtr);
            return Set(std::move(itemPtrCasted), std::move(policy));
        }

        bool Set(CacheItemBase::Ptr itemPtr, ICachePolicy::Ptr policy = nullptr) final;

        template <class TValue> const TValue *Get(const std::string &key) const
        {
            auto item = GetItem(key);
            return item ? item->GetValueAs<TValue>() : nullptr;
        }

        const void *Get(const std::string &key) const final;

        template <class TValue> const CacheItem<TValue> *GetItem(const std::string &key) const
        {
            auto item = GetItem(key);
            return item ? item->Upcast<TValue>() : nullptr;
        }

        const CacheItemBase *GetItem(const std::string &key) const final;

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

    class CacheWrapper
    {
      private:
        Cache _cache;
        std::string _separator;

      public:
        CacheWrapper(const CacheWrapper &) = delete;
        CacheWrapper(CacheWrapper &&) = delete;
        CacheWrapper(const std::string &separator = "::") : _separator(separator) {}

        CacheWrapper &operator=(const CacheWrapper &) = delete;
        CacheWrapper &operator=(CacheWrapper &&) = delete;

        template <class TValue>
        bool Add(const std::string &key, TValue &&value, typename CachePolicy<TValue>::Ptr policy = nullptr)
        {
            return _cache.Add<TValue>(BuildKey<TValue>(key), std::move(value), std::move(policy));
        }

        template <class TValue>
        bool Set(const std::string &key, TValue &&value, typename CachePolicy<TValue>::Ptr policy = nullptr)
        {
            return _cache.Set<TValue>(BuildKey<TValue>(key), std::move(value), std::move(policy));
        }

        template <class TValue> const TValue *Get(const std::string &key) const
        {
            return _cache.Get<TValue>(BuildKey<TValue>(key));
        }

        template <class TValue> const CacheItem<TValue> *GetItem(const std::string &key) const
        {
            return _cache.GetItem<TValue>(BuildKey<TValue>(key));
        }

        template <class TValue> bool Remove(const std::string &key) { return _cache.Remove(BuildKey<TValue>(key)); }

        template <class TValue> bool Contains(const std::string &key) const
        {
            return _cache.Contains(BuildKey<TValue>(key));
        }

        size_t Count() const { return _cache.Count(); }

      private:
        template <class TValue> std::string BuildKey(const std::string &key) const
        {
            std::string name = typeid(TValue).name();
            return name + _separator + key;
        }
    };
} // namespace sd