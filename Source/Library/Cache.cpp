#include "Cache.hpp"
#include <optional>

namespace sd
{
    bool Cache::Add(CacheItemBase::Ptr item, ICachePolicy::Ptr policy)
    {
        if (!item)
        {
            return false;
        }
        return AddData({.item = std::move(item), .policy = std::move(policy)});
    }

    bool Cache::Set(CacheItemBase::Ptr item, ICachePolicy::Ptr newPolicy)
    {
        if (!item)
        {
            return false;
        }
        auto data = GetEditableData(item->GetKey());
        if (!data)
        {
            return false;
        }
        std::swap(data->item, item);
        auto &oldItem = item;
        auto &newItem = data->item;
        auto &oldPolicy = data->policy;
        if (oldPolicy && oldItem && newItem)
        {
            oldPolicy->CallOnUpdate(oldItem.get(), newItem.get());
        }
        if (newPolicy)
        {
            data->policy = std::move(newPolicy);
        }
        return true;
    }

    const void *Cache::Get(const std::string &key) const
    {
        auto item = GetItem(key);
        return item ? item->GetRawValue() : nullptr;
    }

    const CacheItemBase *Cache::GetItem(const std::string &key) const
    {
        if (auto data = GetData(key); data && data->item)
        {
            return data->item.get();
        }
        return nullptr;
    }

    bool Cache::Remove(const std::string &key)
    {
        auto data = RemoveData(key);
        if (!data)
        {
            return false;
        }
        auto &item = data->item;
        auto &policy = data->policy;
        if (item && policy)
        {
            policy->CallOnRemove(item.get());
        }
        return true;
    }

    bool Cache::Contains(const std::string &key) const { return ContainsData(key); }

    size_t Cache::Count() const { return CountData(); }

    Cache::Data *Cache::GetEditableData(const std::string &key) { return const_cast<Data *>(GetData(key)); }

    bool Cache::AddData(Cache::Data data) { return _items.insert({data.item->GetKey(), std::move(data)}).second; }

    std::optional<Cache::Data> Cache::RemoveData(const std::string &key)
    {
        auto it = _items.find(key);
        if (it == _items.end())
        {
            return std::nullopt;
        }
        auto data = std::move(it->second);
        _items.erase(it);
        return std::move(data);
    }

    const Cache::Data *Cache::GetData(const std::string &key) const
    {
        if (auto it = _items.find(key); it != _items.end())
        {
            return &it->second;
        }
        return nullptr;
    }

    bool Cache::ContainsData(const std::string &key) const { return _items.contains(key); }

    size_t Cache::CountData() const { return _items.size(); }
} // namespace sd