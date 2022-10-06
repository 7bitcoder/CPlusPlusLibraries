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

#include "DependencyInjector.hpp"
#include "Reflection.hpp"

namespace sd
{
    template <class T> class Singeleton
    {
      private:
        T *_service = nullptr;

      public:
        using Type = T;

        Singeleton() = default;
        Singeleton(T *service) : _service(service) {}

        T *get() const { return _service; }
    };

    template <int T, class O> class Shared
    {
      private:
        static constexpr int Token = T;
        std::shared_ptr<O> _object;

      public:
        using Type = O;

        static int getToken() { return Token; }

        Shared() = default;
        Shared(std::shared_ptr<O> object) : _object(object) {}

        std::shared_ptr<O> get() const { return _object; }
    };

    template <class T> class Unique
    {
      private:
        std::shared_ptr<T> _object;

      public:
        using Type = T;

        Unique() = default;
        Unique(std::shared_ptr<T> object) : _object(object) {}

        std::shared_ptr<T> get() const { return _object; }
    };

    struct Scope
    {
      public:
        enum Type
        {
            Singeleton = 1,
            Scoped,
            Transient,
        };

      private:
        const Type type;

        Scope(Type type) : type(type) {}

      public:
        static Scope singeleton() { return {Type::Singeleton}; }
        static Scope transient() { return {Type::Transient}; }
        static Scope scoped() { return {Type::Scoped}; }

        bool isSingeleton() const { return type == Type::Singeleton; }
        bool isTransient() const { return type == Type::Transient; }
        bool isScoped() const { return type == Type::Scoped; }
    };

    class DependencyInjector
    {
      private:
        struct IObjectHolder
        {
            virtual std::shared_ptr<void> getObjectPtr() const = 0;

            virtual void destroyObject() = 0;
            virtual bool isValid() const = 0;

            virtual ~IObjectHolder() {}
        };

        template <class T> class ObjectHolder final : public IObjectHolder
        {
          private:
            std::shared_ptr<T> _objectPtr;

            ObjectHolder(T *objectPtr) : _objectPtr(objectPtr) {}

          public:
            ObjectHolder(const ObjectHolder &) = delete;
            ObjectHolder &operator=(const ObjectHolder &) = delete;

            template <class... Args> static std::unique_ptr<ObjectHolder<T>> create(Args &&...params)
            {
                auto objectPtr = new T{std::forward<Args>(params)...};
                return std::unique_ptr<ObjectHolder<T>>(new ObjectHolder{objectPtr});
            };

            std::shared_ptr<T> getTypedObjectPtr() const { return _objectPtr; }
            std::shared_ptr<void> getObjectPtr() const final { return _objectPtr; }

            void destroyObject() { _objectPtr.reset(); }
            bool isValid() const final { return !!getObjectPtr(); }
            operator bool() const { return isValid(); }
        };

        template <class T> struct TypeExtractor
        {
            using Type = T;
        };
        template <class T> struct TypeExtractor<std::shared_ptr<T>>
        {
            using Type = typename std::shared_ptr<T>::element_type;
        };
        template <class T> struct TypeExtractor<std::unique_ptr<T>>
        {
            using Type = typename std::unique_ptr<T>::element_type;
        };
        template <class T> struct TypeExtractor<Singeleton<T>>
        {
            using Type = typename Singeleton<T>::Type;
        };
        template <class T> struct TypeExtractor<Unique<T>>
        {
            using Type = typename Unique<T>::Type;
        };
        template <class T, int U> struct TypeExtractor<Shared<U, T>>
        {
            using Type = typename Shared<U, T>::Type;
        };
        template <class O, int T = 0> struct TokenExtractor
        {
            static constexpr int token = T;
        };
        template <class O, int T> struct TokenExtractor<Shared<T, O>>
        {
            static constexpr int token = T;
        };
        // template <class T, class U> struct IsShared<T> : public std::false_type
        // {
        // };
        // template <class T, class U> struct IsShared<Shared<T, U>> : public std::false_type
        // {
        // };
        template <class T> struct ConstrutorTraits
        {
            using Type = T;
            using TupleArgs = AsTuple<Type>;

            static constexpr size_t ArgsSize = std::tuple_size_v<TupleArgs>;

            template <size_t I> struct Arg
            {
                using Type = typename std::tuple_element<I, TupleArgs>::type;
                using InnerType = typename TypeExtractor<Type>::Type;
                using RawType = typename std::remove_cvref_t<std::remove_pointer_t<InnerType>>;
                using RawTypePtr = typename std::add_pointer_t<RawType>;
                using SharedPtr = std::shared_ptr<RawType>;
                using UniquePtr = std::unique_ptr<RawType>;
                using Singeleton = Singeleton<RawType>;
                using Unique = Unique<RawType>;
                using TokenAccesor = TokenExtractor<Type>;
                using Shared = Shared<TokenAccesor::token, RawType>;
            };
        };

        struct IConstructionInfo
        {
            virtual const Scope &getScope() const = 0;

            virtual std::type_index getTypeIndex() const = 0;

            virtual const std::vector<std::pair<std::type_index, Scope>> getParamsTypeIndexes() const = 0;

            virtual std::unique_ptr<IObjectHolder> construct(
                const std::vector<std::shared_ptr<void>> &params) const = 0;

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

            const std::vector<std::pair<std::type_index, Scope>> getParamsTypeIndexes() const
            {
                return getParamsTypeIndexesInt(Indices{});
            };

            std::unique_ptr<IObjectHolder> construct(const std::vector<std::shared_ptr<void>> &params) const
            {
                if (params.size() != ConstructorTraits::ArgsSize)
                {
                    throw std::runtime_error("Wrong Argument vector size");
                }
                return constructInt(Indices{}, params);
            };

          private:
            template <size_t... I>
            const std::vector<std::pair<std::type_index, Scope>> getParamsTypeIndexesInt(
                std::index_sequence<I...> seq) const
            {
                return std::vector<std::pair<std::type_index, Scope>>{(getParameterInfo<I>())...};
            };

            template <size_t... I>
            std::unique_ptr<ObjectHolder<T>> constructInt(std::index_sequence<I...> seq,
                                                          const std::vector<std::shared_ptr<void>> &params) const
            {
                return ObjectHolder<T>::create(getParameter<I>(params)...);
            };

            template <size_t I> std::pair<std::type_index, Scope> getParameterInfo() const
            {
                using Type = typename ConstructorTraits::template Arg<I>::Type;
                using RawType = typename ConstructorTraits::template Arg<I>::RawType;
                using SharedPtr = typename ConstructorTraits::template Arg<I>::SharedPtr;

                std::type_index typeIndex = typeid(RawType);

                if constexpr (std::is_same_v<Type, SharedPtr>)
                {
                    return {typeIndex, Scope::singeleton()};
                }
                else if constexpr (std::is_same_v<Type, typename ConstructorTraits::template Arg<I>::UniquePtr>)
                {
                    return {typeIndex, Scope::unique()};
                }
                else if constexpr (std::is_same_v<Type, typename ConstructorTraits::template Arg<I>::RawTypePtr>)
                {
                    return {typeIndex, Scope::singeleton()};
                }
                else if constexpr (std::is_same_v<Type, typename ConstructorTraits::template Arg<I>::Singeleton>)
                {
                    return {typeIndex, Scope::singeleton()};
                }
                else if constexpr (std::is_same_v<Type, typename ConstructorTraits::template Arg<I>::Unique>)
                {
                    return {typeIndex, Scope::unique()};
                }
                else if constexpr (std::is_same_v<Type, typename ConstructorTraits::template Arg<I>::Shared>)
                {
                    return {typeIndex, Scope::shared(ConstructorTraits::template Arg<I>::TokenAccesor::token)};
                }
                else
                {
                    return {typeIndex, Scope::singeleton()};
                }
            }

            template <size_t I> constexpr auto getParameter(const std::vector<std::shared_ptr<void>> &params) const
            {
                using Type = typename ConstructorTraits::template Arg<I>::Type;
                using RawType = typename ConstructorTraits::template Arg<I>::RawType;
                using SharedPtr = typename ConstructorTraits::template Arg<I>::SharedPtr;
                using Shared = typename ConstructorTraits::template Arg<I>::Shared;

                SharedPtr typedShared = std::static_pointer_cast<RawType>(params[I]);
                if constexpr (std::is_same_v<Type, SharedPtr>)
                {
                    return typedShared;
                }
                else if constexpr (std::is_same_v<Type, typename ConstructorTraits::template Arg<I>::UniquePtr>)
                {
                    return typedShared;
                }
                else if constexpr (std::is_same_v<Type, typename ConstructorTraits::template Arg<I>::RawTypePtr>)
                {
                    return typedShared.get();
                }
                else if constexpr (std::is_same_v<Type, typename ConstructorTraits::template Arg<I>::Singeleton>)
                {
                    using Singeleton = typename ConstructorTraits::template Arg<I>::Singeleton;
                    return Singeleton{typedShared.get()};
                }
                else if constexpr (std::is_same_v<Type, typename ConstructorTraits::template Arg<I>::Unique>)
                {
                    using Unique = typename ConstructorTraits::template Arg<I>::Unique;
                    return Unique{typedShared};
                }
                else if constexpr (std::is_same_v<Type, typename ConstructorTraits::template Arg<I>::Shared>)
                {
                    using Shared = typename ConstructorTraits::template Arg<I>::Shared;
                    return Shared{typedShared};
                }
                else
                {
                    Shared ff = 123;
                    return nullptr;
                }
            }
        };

        struct ObjKey
        {
            const std::type_index typeIndex;
            int token;

            ObjKey(const ObjKey &) = default;
            ObjKey(std::type_index typeIndex, int token) : typeIndex(typeIndex), token(token) {}

            bool operator==(const ObjKey &other) const { return typeIndex == other.typeIndex && token == other.token; }
        };

        struct ObjKeyHasher
        {
            std::size_t operator()(const ObjKey &k) const
            {

                return ((std::hash<std::type_index>()(k.typeIndex) ^ (std::hash<int>()(k.token) << 1)) >> 1);
            }
        };

      private:
        std::unordered_map<std::type_index, std::unique_ptr<IConstructionInfo>> _registered;
        std::unordered_map<ObjKey, std::unique_ptr<IObjectHolder>, ObjKeyHasher> _objectsMap;

      public:
        template <class I, class T> void addSingeleton() { add<I, T>(Scope::singeleton()); }
        template <class I, class T> void addTransient() { add<I, T>(Scope::transient()); }
        template <class I, class T> void addScoped() { add<I, T>(Scope::scoped()); }

        template <class I> std::shared_ptr<I> get() { return (I *)getFromObjectsMap(typeid(I)).get(); }

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
        template <class I, class T> void add(const Scope &scope)
        {
            static_assert(std::is_base_of_v<I, T>, "Type T must inherit from I");
            _registered.insert({typeid(I), std::make_unique<ConstructionInfo<T>>(scope)});
        }

        std::shared_ptr<void> createAndRegister(std::type_index typeIndex, const Scope &scope)
        {
            auto objectHolder = create(typeIndex);
            auto ptr = objectHolder->getObjectPtr();
            ObjKey key{typeIndex, scope.isShared() ? scope.getToken() : 0};
            _objectsMap.insert({key, std::move(objectHolder)});
            return ptr;
        }

        std::unique_ptr<IObjectHolder> create(std::type_index typeIndex)
        {
            std::vector<std::shared_ptr<void>> params;
            auto &info = getRegisteredSafe(typeIndex);
            auto &indexes = info.getParamsTypeIndexes();
            for (auto pair : indexes)
            {
                auto [index, scope] = pair;
                params.push_back(get(index, scope));
            }
            return info.construct(params);
        }

        std::shared_ptr<void> get(std::type_index index, const Scope &scope)
        {
            auto &info = getRegisteredSafe(index);

            auto registeredScope = info.getScope();
            if (scope.isUnique())
            {
                return create(index);
            }
            if (scope.isSingeleton() && registeredScope.isSingeleton())
            {
                return getFromObjectsMap(index);
            }
            if (scope.isShared() && (registeredScope.isShared() || registeredScope.isSingeleton()))
            {
                int token = scope.getToken();
                if (auto object = getFromObjectsMap(index, token))
                {
                    return object;
                }
                return createAndRegister(index, scope);
            }
            throw std::runtime_error("Services build failed");
        }

        std::shared_ptr<void> getFromObjectsMap(std::type_index index, int token = 0)
        {
            auto pair = _objectsMap.find({index, token});
            return pair != _objectsMap.end() ? pair->second->getObjectPtr() : nullptr;
        }

        std::shared_ptr<void> getFromObjectsMapSafe(std::type_index index, int token = 0)
        {
            if (auto ptr = getFromObjectsMap(index, token))
            {
                return ptr;
            }
            throw std::exception_ptr();
        }

        IConstructionInfo *getRegistered(std::type_index index)
        {
            auto pair = _registered.find(index);
            return pair != _registered.end() ? pair->second.get() : nullptr;
        }

        IConstructionInfo &getRegisteredSafe(std::type_index index)
        {
            if (auto ptr = getRegistered(index))
            {
                return *ptr;
            }
            throw std::runtime_error("Object type was not registered");
        }
    };
} // namespace sd