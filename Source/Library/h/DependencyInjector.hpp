#pragma once

#include <algorithm>
#include <exception>
#include <functional>
#include <memory>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>

#include "Reflection.hpp"

namespace sd
{

    class DependencyInjector
    {
      private:
        struct IObjectHolder
        {
            virtual void *getObjectPtr() const = 0;

            virtual void destroyObject() = 0;
            virtual bool isValid() const = 0;

            virtual ~IObjectHolder() {}
        };

        template <class T> class ObjectHolder final : public IObjectHolder
        {
          private:
            std::unique_ptr<T> _objectPtr;

            ObjectHolder(T *objectPtr) : _objectPtr(objectPtr) {}

          public:
            ObjectHolder(const ObjectHolder &) = delete;
            ObjectHolder &operator=(const ObjectHolder &) = delete;

            template <class... Args> static std::unique_ptr<ObjectHolder<T>> create(Args &&...params)
            {
                auto objectPtr = new T{std::forward<Args>(params)...};
                return std::unique_ptr<ObjectHolder<T>>(new ObjectHolder{objectPtr});
            };

            T *getTypedObjectPtr() const { return _objectPtr.get(); }
            void *getObjectPtr() const final { return _objectPtr.get(); }

            void destroyObject() { _objectPtr.reset(); }
            bool isValid() const final { return !!getObjectPtr(); }
            operator bool() const { return isValid(); }
        };

        template <class T> struct ConstrutorTraits
        {
            using Type = T;
            using TupleArgs = AsTuple<Type>;

            static constexpr size_t ArgsSize = std::tuple_size_v<TupleArgs>;

            template <size_t I> struct Arg
            {
                using Type = typename std::tuple_element<I, TupleArgs>::type;
                using RawType = typename std::remove_cvref_t<std::remove_pointer_t<Type>>;
                using RawTypePtr = typename std::add_pointer_t<RawType>;
            };
        };

        struct IConstructionInfo
        {
            virtual std::type_index getTypeIndex() const = 0;

            virtual const std::vector<std::type_index> getParamsTypeIndexes() const = 0;

            virtual std::unique_ptr<IObjectHolder> construct(const std::vector<void *> &params) const = 0;

            virtual ~IConstructionInfo() {}
        };

        template <typename T> class ConstructionInfo final : public IConstructionInfo
        {
          private:
            using ConstructorTraits = ConstrutorTraits<T>;
            using Indices = std::make_index_sequence<ConstructorTraits::ArgsSize>;

          public:
            std::type_index getTypeIndex() const { return typeid(typename ConstructorTraits::Type); }

            const std::vector<std::type_index> getParamsTypeIndexes() const
            {
                return getParamsTypeIndexesInt(Indices{});
            };

            std::unique_ptr<IObjectHolder> construct(const std::vector<void *> &params) const
            {
                if (params.size() != ConstructorTraits::ArgsSize)
                {
                    throw std::runtime_error("Wrong Argument vector size");
                }
                return constructInt(Indices{}, params);
            };

          private:
            template <size_t... I>
            const std::vector<std::type_index> getParamsTypeIndexesInt(std::index_sequence<I...> seq) const
            {
                return std::vector<std::type_index>{(typeid(typename ConstructorTraits::template Arg<I>::RawType))...};
            };

            template <size_t... I>
            std::unique_ptr<ObjectHolder<T>> constructInt(std::index_sequence<I...> seq,
                                                          const std::vector<void *> &params) const
            {
                return ObjectHolder<T>::create(getParameter<I>(params)...);
            };

            template <size_t I> constexpr auto getParameter(const std::vector<void *> &params) const
            {
                if constexpr (std::is_pointer_v<typename ConstructorTraits::template Arg<I>::Type>)
                {
                    return (typename ConstructorTraits::template Arg<I>::RawTypePtr)params[I];
                }
                else if constexpr (std::is_reference_v<typename ConstructorTraits::template Arg<I>::Type>)
                {
                    return *(typename ConstructorTraits::template Arg<I>::RawTypePtr)params[I];
                }
                else
                {
                    static_assert(true, "Could not fetch parameters");
                }
            }
        };

      private:
        std::unordered_map<std::type_index, std::unique_ptr<IConstructionInfo>> _registered;
        std::unordered_map<std::type_index, std::unique_ptr<IObjectHolder>> _objectsMap;

      public:
        template <class I, class T> void addSingeleton()
        {
            static_assert(std::is_base_of_v<I, T>, "Type T must inherit from I");
            _registered.insert({typeid(I), std::make_unique<ConstructionInfo<T>>()});
        }

        template <class I> I *getPtr() { return (I *)get(typeid(I)); }
        template <class I> I &getRef()
        {
            if (auto ptr = getPtr<I>())
            {
                return *ptr;
            }
            throw std::exception_ptr();
        }

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
                    throw std::runtime_error("Services build failed");
                }
            }
            auto objectHolder = info.construct(params);
            auto rawPtr = objectHolder->getObjectPtr();
            _objectsMap.insert({typeIndex, std::move(objectHolder)});
            return rawPtr;
        }

        void *get(std::type_index index)
        {
            auto pair = _objectsMap.find(index);
            return pair != _objectsMap.end() ? pair->second->getObjectPtr() : nullptr;
        }

        IConstructionInfo *getRegistered(std::type_index index)
        {
            auto pair = _registered.find(index);
            return pair != _registered.end() ? pair->second.get() : nullptr;
        }
    };
} // namespace sd