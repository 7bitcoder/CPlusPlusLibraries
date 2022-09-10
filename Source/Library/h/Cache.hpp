#pragma once
#include <cstddef>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>

namespace sd
{

    template <class T> class CacheItem;

    struct ICacheItem
    {
        using Ptr = std::unique_ptr<ICacheItem>;

        template <class T> const CacheItem<T> *Upcast() const { return dynamic_cast<const CacheItem<T> *>(this); }

        template <class T> CacheItem<T> *Upcast() { return dynamic_cast<CacheItem<T> *>(this); }

        template <class T> const T *As() const
        {
            auto casted = Upcast<T>();
            return casted ? casted->Typed() : nullptr;
        }

        template <class T> T *As()
        {
            auto casted = Upcast<T>();
            return casted ? casted->Typed() : nullptr;
        }

        virtual const void *Raw() const = 0;
        virtual void *Raw() = 0;

        virtual ~ICacheItem(){};
    };

    template <class T> struct CacheItem : public ICacheItem
    {
      private:
        T _value;

      public:
        using Ptr = std::unique_ptr<CacheItem<T>>;

        static Ptr Make(T &&value) { return Ptr(new CacheItem<T>(std::move(value))); }

        CacheItem(T &&value) : _value(std::move(value)) {}

        const void *Raw() const { return &_value; }

        void *Raw() { return &_value; }

        const T *Typed() const { return &_value; }

        T *Typed() { return &_value; }

        const T &operator->() const { return _value; }

        const T &operator*() const { return _value; }

        T &operator->() { return _value; }

        T &operator*() { return _value; }
    };

    struct ICachePolicy
    {
        using Ptr = std::unique_ptr<ICachePolicy>;

        virtual void CallOnRemove(const ICacheItem *value) const = 0;
        virtual void CallOnUpdate(const ICacheItem *oldValue, const ICacheItem *newValue) const = 0;

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

        void CallOnRemove(const ICacheItem *value) const final
        {
            if (_removeCallback && value)
            {
                _removeCallback(value->As<TValue>());
            }
        }

        void CallOnUpdate(const ICacheItem *oldValue, const ICacheItem *newValue) const final
        {
            if (_updateCallback && oldValue && newValue)
            {
                _updateCallback(oldValue->As<TValue>(), newValue->As<TValue>());
            }
        }
    };

    struct ICache
    {
        virtual bool Add(const std::string &key, ICacheItem::Ptr value, ICachePolicy::Ptr policy = nullptr) = 0;

        virtual bool Set(const std::string &key, ICacheItem::Ptr value, ICachePolicy::Ptr policy = nullptr) = 0;

        virtual const void *Get(const std::string &key) const = 0;

        virtual const ICacheItem *GetItem(const std::string &key) const = 0;

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
            ICacheItem::Ptr item;
            ICachePolicy::Ptr policy;
        };

        std::unordered_map<std::string, Data> _items;

      public:
        Cache() = default;
        Cache(const Cache &) = delete;
        Cache(Cache &&) = delete;

        Cache &operator=(const Cache &) = delete;
        Cache &operator=(Cache &&) = delete;

        template <class T> bool Add(const std::string &key, T &&value, typename CachePolicy<T>::Ptr policy = nullptr)
        {
            return Add<T>(key, CacheItem<T>::Make(std::move(value)), std::move(policy));
        }

        template <class T>
        bool Add(const std::string &key, typename CacheItem<T>::Ptr itemPtr,
                 typename CachePolicy<T>::Ptr policy = nullptr)
        {
            ICacheItem::Ptr itemPtrCasted = std::move(itemPtr);
            return Add(key, std::move(itemPtrCasted), std::move(policy));
        }

        bool Add(const std::string &key, ICacheItem::Ptr itemPtr, ICachePolicy::Ptr policy = nullptr) final;

        template <class T> bool Set(const std::string &key, T &&value, typename CachePolicy<T>::Ptr policy = nullptr)
        {
            return Set<T>(key, CacheItem<T>::Make(std::move(value)), std::move(policy));
        }

        template <class T>
        bool Set(const std::string &key, typename CacheItem<T>::Ptr itemPtr,
                 typename CachePolicy<T>::Ptr policy = nullptr)
        {
            ICacheItem::Ptr itemPtrCasted = std::move(itemPtr);
            return Set(key, std::move(itemPtrCasted), std::move(policy));
        }

        bool Set(const std::string &key, ICacheItem::Ptr itemPtr, ICachePolicy::Ptr policy = nullptr) final;

        template <class TValue> const TValue *Get(const std::string &key) const
        {
            auto item = GetItem(key);
            return item ? item->As<TValue>() : nullptr;
        }

        const void *Get(const std::string &key) const final;

        template <class TValue> const CacheItem<TValue> *GetItem(const std::string &key) const
        {
            auto item = GetItem(key);
            return item ? item->Upcast<TValue>() : nullptr;
        }

        const ICacheItem *GetItem(const std::string &key) const final;

        bool Remove(const std::string &key) final;

        bool Contains(const std::string &key) const final;

        size_t Count() const final;

      private:
        bool AddData(const std::string &key, Data data);

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