#pragma once

#include <algorithm>
#include <functional>
#include <memory>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>

namespace sd
{

    class DependencyInjector
    {
      private:
        template <class T> struct Constructor
        {
            T ctor;
            Constructor(T ctor) : ctor(ctor) {}
        };

        struct IConstructionInfo
        {
            virtual std::type_index getTypeIndex() const = 0;

            virtual const std::vector<std::type_index> &getParamsTypeIndexes() const = 0;

            virtual void *construct(const std::vector<void *> &params) const = 0;

            virtual ~IConstructionInfo() {}
        };

        template <class Type, class Param1> class ConstructionInfo final : public IConstructionInfo
        {
          private:
            std::type_index _typeIndex = typeid(Type);
            std::vector<std::type_index> _params;
            Type *(*_ctor)(Param1 *);

          public:
            ConstructionInfo(Constructor<Type *(*)(Param1 *)> constructor)
            {
                _ctor = constructor.ctor;
                _params = {typeid(Param1)};
            }

            std::type_index getTypeIndex() const { return _typeIndex; }

            const std::vector<std::type_index> &getParamsTypeIndexes() const { return _params; };

            void *construct(const std::vector<void *> &params) const { return (*_ctor)((Param1 *)params[0]); };
        };

      private:
        std::unordered_map<std::type_index, std::unique_ptr<IConstructionInfo>> _registered;
        std::unordered_map<std::type_index, void *> _objectsMap;

      public:
        template <class I, class T> void addSingeleton()
        {
            // todo check if T inherits from/implements I
            auto cons = Constructor{T::constructor};
            auto i = new ConstructionInfo(cons);
            _registered.insert({typeid(I), std::unique_ptr<IConstructionInfo>(i)});
        }

        template <class I> I *get() { return (I *)get(typeid(I)); }

        void build()
        {
            for (auto &objectToBuild : _registered)
            {
                auto &[index, constructionInfo] = objectToBuild;
                if (!get(index))
                {
                    make(index, *constructionInfo);
                }
            }
        }

      private:
        void *make(std::type_index typeIndex, IConstructionInfo &info)
        {
            std::vector<void *> params;
            auto &indexes = info.getParamsTypeIndexes();
            for (auto index : indexes)
            {
                if (auto object = get(index))
                {
                    params.push_back(object);
                }
                else if (auto constructionInfo = getRegistered(index))
                {
                    params.push_back(make(index, *constructionInfo));
                }
                else
                {
                    // todo
                }
            }
            auto object = info.construct(params);
            _objectsMap.insert({typeIndex, object});
            return object;
        }

        void *get(std::type_index index)
        {
            auto pair = _objectsMap.find(index);
            return pair != _objectsMap.end() ? pair->second : nullptr;
        }

        IConstructionInfo *getRegistered(std::type_index index)
        {
            auto pair = _registered.find(index);
            return pair != _registered.end() ? pair->second.get() : nullptr;
        }
    };
} // namespace sd