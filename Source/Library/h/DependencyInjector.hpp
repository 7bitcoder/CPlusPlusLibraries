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
        template <typename T> struct FunctionTraits;

        template <typename R, typename... Args> struct FunctionTraits<std::function<R(Args...)>>
        {
            static const size_t nargs = sizeof...(Args);

            typedef R result_type;

            template <size_t i> struct arg
            {
                typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
            };
        };

        struct IConstructionInfo
        {
            virtual std::type_index getTypeIndex() const = 0;

            virtual const std::vector<std::type_index> getParamsTypeIndexes() const = 0;

            virtual void *construct(const std::vector<void *> &params) const = 0;

            virtual ~IConstructionInfo() {}
        };

        template <typename R, typename... Args> class ConstructionInfo final : public IConstructionInfo
        {
          private:
            using ConstructorTraits = FunctionTraits<std::function<R(Args...)>>;
            std::function<R(Args...)> _constructor;

          public:
            ConstructionInfo(const std::function<R(Args...)> &constructor) { _constructor = constructor; }

            std::type_index getTypeIndex() const { return typeid(typename ConstructorTraits::result_type); }

            const std::vector<std::type_index> getParamsTypeIndexes() const
            {
                if constexpr (ConstructorTraits::nargs == 0)
                {
                    return {};
                }
                else if constexpr (ConstructorTraits::nargs == 1)
                {
                    return {typeid(typename ConstructorTraits::template arg<0>::type)};
                }
                else if constexpr (ConstructorTraits::nargs == 2)
                {
                    return {typeid(typename ConstructorTraits::template arg<0>::type),
                            typeid(typename ConstructorTraits::template arg<1>::type)};
                }
            };

            void *construct(const std::vector<void *> &params) const
            {
                if constexpr (ConstructorTraits::nargs == 0)
                {
                    return _constructor();
                }
                else if constexpr (ConstructorTraits::nargs == 1)
                {
                    return _constructor((typename ConstructorTraits::template arg<0>::type)(params[0]));
                }
                else if constexpr (ConstructorTraits::nargs == 2)
                {
                }
            };
        };

      private:
        std::unordered_map<std::type_index, std::unique_ptr<IConstructionInfo>> _registered;
        std::unordered_map<std::type_index, void *> _objectsMap;

      public:
        template <class I, class T> void addSingeleton()
        {
            // todo check if T inherits from/implements I
            std::function<decltype(T::constructor)> ff{&T::constructor};
            auto i = new ConstructionInfo(ff);
            _registered.insert({typeid(I *), std::unique_ptr<IConstructionInfo>(i)});
        }

        template <class I> I *get() { return (I *)get(typeid(I *)); }

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
            if (pair != _registered.end())
            {
                auto &ob = pair->second;
                return ob.get();
            }
            else
            {
                return nullptr;
            }
        }
    };
} // namespace sd