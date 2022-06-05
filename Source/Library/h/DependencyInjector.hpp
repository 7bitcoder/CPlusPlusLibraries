#pragma once

#include <algorithm>
#include <exception>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <utility>

#include "Reflection.hpp"

namespace sd
{
    template <const char *T, class S> class Token
    {
      private:
        static constexpr const char *Value = T;
        S *_object;

      public:
        static std::string getToken() { return Value; }

        Token(S *object) : _object(object) {}

        S *get() const { return _object; }
    };

    enum ScopeType
    {
        Singeleton,
        Tokenized,
        Scoped,
    };

    struct Scope
    {
      public:
        static Scope singeleton() { return {ScopeType::Singeleton}; }
        static Scope scoped() { return {ScopeType::Scoped}; }
        static Scope tokenized(const char *token) { return {token}; }

        const ScopeType type;
        const char *token = nullptr;

        Scope(ScopeType type) : type(type) {}
        Scope(const char *token) : type(ScopeType::Tokenized), token(token) {}

        bool isSingeleton() const { return type == ScopeType::Singeleton; }
        bool isScoped() const { return type == ScopeType::Scoped; }
        bool isTokenized() const { return type == ScopeType::Tokenized; }
    };

    class DependencyInjector
    {
      private:
        struct IObjectHolder
        {
            virtual void *getObjectPtr() const = 0;

            virtual void destroyObject() = 0;
            virtual bool isValid() const = 0;

            virtual const char *getToken() const = 0;

            virtual ~IObjectHolder() {}
        };

        template <class T> class ObjectHolder final : public IObjectHolder
        {
          private:
            std::unique_ptr<T> _objectPtr;
            const char *_token = nullptr;

            ObjectHolder(T *objectPtr, const char *token = nullptr) : _objectPtr(objectPtr), _token(token) {}

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
            virtual const Scope &getScope() const = 0;

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
            Scope _scope;

          public:
            ConstructionInfo(const Scope &scope) : _scope(scope) {}

            const Scope &getScope() const { return _scope; };

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
        using ObjKey = std::pair<std::type_index, const char *>;
        std::unordered_map<std::type_index, std::unique_ptr<IConstructionInfo>> _registered;
        std::unordered_map<ObjKey, std::unique_ptr<IObjectHolder>> _objectsMap;

      public:
        template <class I, class T> void addSingeleton()
        {
            static_assert(std::is_base_of_v<I, T>, "Type T must inherit from I");
            _registered.insert({typeid(I), std::make_unique<ConstructionInfo<T>>(Scope::singeleton())});
        }
        template <class I, class T> void addScoped()
        {
            static_assert(std::is_base_of_v<I, T>, "Type T must inherit from I");
            _registered.insert({typeid(I), std::make_unique<ConstructionInfo<T>>(Scope::scoped())});
        }
        template <class I, class T> void addTokenized(const char *token)
        {
            static_assert(std::is_base_of_v<I, T>, "Type T must inherit from I");
            _registered.insert({typeid(I), std::make_unique<ConstructionInfo<T>>(Scope::tokenized(token))});
        }

        template <class I> I *getPtr(const char *token = nullptr) { return (I *)getFromObjectsMap(typeid(I), token); }
        template <class I> I &getRef(const char *token = nullptr)
        {
            if (auto ptr = getPtr<I>(token))
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
                if (constructionInfo->getScope().isSingeleton() && !getFromObjectsMap(index))
                {
                    createAndRegister(index, constructionInfo->getScope());
                }
            }
        }

      private:
        void *createAndRegister(std::type_index typeIndex, const Scope &scope)
        {
            auto objectHolder = create(typeIndex);
            auto rawPtr = objectHolder->getObjectPtr();
            ObjKey key{typeIndex, scope.isTokenized() ? scope.token : nullptr};
            _objectsMap.insert({key, std::move(objectHolder)});
            return rawPtr;
        }

        std::unique_ptr<IObjectHolder> create(std::type_index typeIndex)
        {
            std::vector<void *> params;
            auto info = getRegistered(typeIndex);
            if (!info)
            {
                // todo
            }
            auto &indexes = info->getParamsTypeIndexes();
            for (auto index : indexes)
            {
                params.push_back(get(index));
            }
            return info->construct(params);
        }

        void *get(std::type_index index, const Scope &scope = Scope::singeleton())
        {
            if (scope.isScoped())
            {
                return create(index);
            }
            const char *token = scope.isTokenized() ? scope.token : nullptr;
            if (auto object = getFromObjectsMap(index, token))
            {
                return object;
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

        void *getFromObjectsMap(std::type_index index, const char *token = nullptr)
        {
            auto pair = _objectsMap.find({index, token});
            return pair != _objectsMap.end() ? pair->second->getObjectPtr() : nullptr;
        }

        IConstructionInfo *getRegistered(std::type_index index)
        {
            auto pair = _registered.find(index);
            return pair != _registered.end() ? pair->second.get() : nullptr;
        }
    };
} // namespace sd