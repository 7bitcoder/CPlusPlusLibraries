#include "Cache.hpp"
#include <optional>

namespace sd
{
    bool Cache::Add(std::unique_ptr<ICacheItem> itemPtr, std::unique_ptr<ICachePolicy> policy)
    {
        if (!itemPtr)
        {
            return false;
        }
        return AddData({.item = std::move(itemPtr), .policy = std::move(policy)});
    }

    const void *Cache::AddOrGetExisting(std::unique_ptr<ICacheItem> itemPtr, std::unique_ptr<ICachePolicy> policy)
    {
        if (!itemPtr)
        {
            return nullptr;
        }
        if (auto value = Get(itemPtr->GetKey()))
        {
            return value;
        }
        Add(std::move(itemPtr), std::move(policy));
        return nullptr;
    }

    bool Cache::Set(std::unique_ptr<ICacheItem> itemPtr, std::unique_ptr<ICachePolicy> policy)
    {
        if (!itemPtr)
        {
            return false;
        }
        auto data = GetEditableData(itemPtr->GetKey());
        if (!data)
        {
            return false;
        }
        itemPtr.swap(data->item);
        if (data->policy && data->item && itemPtr)
        {
            data->policy->CallOnUpdate(itemPtr->GetValue(), data->item->GetValue());
        }
        if (policy)
        {
            data->policy = std::move(policy);
        }
        return true;
    }

    const void *Cache::Get(const std::string &key) const
    {
        if (auto data = GetData(key); data && data->item)
        {
            return data->item->GetValue();
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
            policy->CallOnRemove(item->GetValue());
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