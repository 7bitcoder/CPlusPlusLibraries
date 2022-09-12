#pragma once
#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>

namespace sd
{

    template <class TValue> class CacheItem;
    struct CacheItemBase
    {
        using UPtr = std::unique_ptr<CacheItemBase>;

        virtual const std::string &GetKey() const = 0;
        virtual const void *GetRawValue() const = 0;

        template <class TValue> const CacheItem<TValue> *Upcast() const
        {
            return dynamic_cast<const CacheItem<TValue> *>(this);
        }

        template <class TValue> const TValue *GetValueAs() const
        {
            auto casted = Upcast<TValue>();
            return casted ? casted->GetValue() : nullptr;
        }

        virtual ~CacheItemBase(){};
    };

    template <class TValue> class CacheItem final : public CacheItemBase
    {
      private:
        std::string _key;
        TValue _value;

      public:
        using UPtr = std::unique_ptr<CacheItem<TValue>>;

        CacheItem(const std::string &key, TValue &&value) : _key(key), _value(std::move(value)) {}

        const std::string &GetKey() const final { return _key; }
        const void *GetRawValue() const final { return GetValue(); }
        const TValue *GetValue() const { return &_value; }
    };

    template <class TValue> typename CacheItem<TValue>::UPtr MakeCacheItem(const std::string &key, TValue &&value)
    {
        return typename CacheItem<TValue>::UPtr(new CacheItem<TValue>(key, std::move(value)));
    }

    struct ICachePolicy
    {
        using UPtr = std::unique_ptr<ICachePolicy>;

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
        using UPtr = std::unique_ptr<CachePolicy<TValue>>;

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

    template <class TValue>
    typename CachePolicy<TValue>::UPtr MakeCachePolicy(
        typename CachePolicy<TValue>::UpdateCallback updateCallback = nullptr,
        typename CachePolicy<TValue>::RemoveCallback removeCallback = nullptr)
    {
        return typename CachePolicy<TValue>::UPtr(new CachePolicy<TValue>(updateCallback, removeCallback));
    }

    struct ICache
    {
        virtual bool Add(CacheItemBase::UPtr item, ICachePolicy::UPtr policy = nullptr) = 0;

        virtual bool Set(CacheItemBase::UPtr item, ICachePolicy::UPtr policy = nullptr) = 0;

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
            CacheItemBase::UPtr item;
            ICachePolicy::UPtr policy;
        };

        std::unordered_map<std::string, Data> _items;

      public:
        Cache() = default;
        Cache(const Cache &) = delete;
        Cache(Cache &&) = delete;

        Cache &operator=(const Cache &) = delete;
        Cache &operator=(Cache &&) = delete;

        template <class TValue>
        bool Add(const std::string &key, TValue &&value, typename CachePolicy<TValue>::UPtr policy = nullptr)
        {
            return Add<TValue>(MakeCacheItem(key, std::move(value)), std::move(policy));
        }

        template <class TValue>
        bool Add(typename CacheItem<TValue>::UPtr item, typename CachePolicy<TValue>::UPtr policy = nullptr)
        {
            CacheItemBase::UPtr itemCasted = std::move(item);
            return Add(std::move(itemCasted), std::move(policy));
        }

        bool Add(CacheItemBase::UPtr item, ICachePolicy::UPtr policy = nullptr) final;

        template <class TValue>
        bool Set(const std::string &key, TValue &&value, typename CachePolicy<TValue>::UPtr policy = nullptr)
        {
            return Set<TValue>(MakeCacheItem(key, std::move(value)), std::move(policy));
        }

        template <class TValue>
        bool Set(typename CacheItem<TValue>::UPtr item, typename CachePolicy<TValue>::UPtr policy = nullptr)
        {
            CacheItemBase::UPtr itemCasted = std::move(item);
            return Set(std::move(itemCasted), std::move(policy));
        }

        bool Set(CacheItemBase::UPtr item, ICachePolicy::UPtr policy = nullptr) final;

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
        bool Add(const std::string &key, TValue &&value, typename CachePolicy<TValue>::UPtr policy = nullptr)
        {
            return _cache.Add<TValue>(BuildKey<TValue>(key), std::move(value), std::move(policy));
        }

        template <class TValue>
        bool Set(const std::string &key, TValue &&value, typename CachePolicy<TValue>::UPtr policy = nullptr)
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