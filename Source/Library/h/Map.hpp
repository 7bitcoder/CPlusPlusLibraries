#pragma once
#include <format>
#include <iostream>
#include <tuple>
#include <utility>

namespace sd
{
    enum Color : char
    {
        Black,
        Red
    };

    template <class K, class T> class MapNode
    {
      public:
        using KeyType = K;
        using ItemType = T;
        using MapNodePtr = MapNode<K, T> *;
        using ConstMapNodePtr = const MapNode<K, T> *;
        using Pair = std::pair<const K, T>;

      private:
        Pair _keyItem;
        Color _color = Color::Black;
        MapNodePtr _parent = nullptr;
        MapNodePtr _left = nullptr;
        MapNodePtr _right = nullptr;

      public:
        MapNode() = delete;

        MapNode(const Pair &p) : _keyItem{p} {}
        MapNode(Pair &&p) : _keyItem{std::move(p)} {}
        // template <class... Args1, class... Args2>
        // MapNode(std::tuple<Args1...> args1, std::tuple<Args2...> args2)
        //     : _keyItem(std::piecewise_construct, args1, args2)
        // {
        // }
        MapNode(const K &k, const T &i) : _keyItem{k, i} {}
        MapNode(K &&k, T &&i) : _keyItem{std::move(k), std::move(i)} {}

        ~MapNode() = default;

        void setRight(MapNodePtr p) { _right = p; }

        MapNodePtr getRight() { return _right; }

        ConstMapNodePtr getRight() const { return _right; }

        bool isRightEmpty() const { return !_right; }

        void setLeft(MapNodePtr p) { _left = p; }

        MapNodePtr getLeft() { return _left; }

        ConstMapNodePtr getLeft() const { return _left; }

        bool isLeftEmpty() const { return !_left; }

        void setParent(MapNodePtr p) { _parent = p; }

        MapNodePtr getParent() { return _parent; }

        ConstMapNodePtr getParent() const { return _parent; }

        const K &getKey() const { return _keyItem.first; }

        T &getItem() { return _keyItem.second; }

        const T &getItem() const { return _keyItem.second; }

        std::pair<const K, T> &getPair() { return _keyItem; }

        const std::pair<const K, T> &getPair() const { return _keyItem; }

        Color getColor() const { return _color; }

        void setColor(Color color) { _color = color; }
    };

    template <class K, class T> class Map;

    template <class K, class T, bool C, bool R> // C= const, R = Reverse
    class MapIterator
    {
      public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using pointer = T *;
        using reference = T &;
        using MapNode = std::conditional_t<C, const MapNode<K, T>, MapNode<K, T>>;
        using MapNodePtr = MapNode *;
        using Pair = std::conditional_t<C, const std::pair<const K, T>, std::pair<const K, T>>;
        using PairRef = Pair &;
        using PairPtr = Pair *;

      protected:
        MapNodePtr _ptr = nullptr;
        const Map<K, T> *const _parentMap;

      public:
        MapIterator(const Map<K, T> *const parentMap, MapNodePtr ptr) : _parentMap(parentMap) { _ptr = ptr; }
        MapIterator(const MapIterator<K, T, C, R> &rawIterator) = default;
        ~MapIterator() = default;

        MapIterator<K, T, C, R> &operator=(const MapIterator<K, T, C, R> &rawIterator) = default;

        operator bool() const { return !_parentMap->isGuard(_ptr); }

        bool operator==(const MapIterator<K, T, C, R> &rawIterator) const { return _ptr == rawIterator._ptr; }
        bool operator!=(const MapIterator<K, T, C, R> &rawIterator) const { return _ptr != rawIterator._ptr; }

        MapIterator<K, T, C, R> &operator++()
        {

            if constexpr (R)
            {
                previous();
            }
            else
            {
                next();
            }

            return (*this);
        }

        MapIterator<K, T, C, R> &operator--()
        {
            if constexpr (R)
            {
                next();
            }
            else
            {
                previous();
            }
            return (*this);
        }

        MapIterator<K, T, C, R> operator++(int)
        {
            auto temp(*this);
            ++*this;
            return temp;
        }

        MapIterator<K, T, C, R> operator--(int)
        {
            auto temp(*this);
            --*this;
            return temp;
        }

        PairRef operator*() { return _ptr->getPair(); }
        PairRef operator*() const { return _ptr->getPair(); }

        PairPtr operator->() { return &_ptr->getPair(); }
        PairPtr operator->() const { return &_ptr->getPair(); }

      private:
        void next()
        {
            if (!_parentMap->isGuard(_ptr->getRight()))
            {
                _ptr = _ptr->getRight();
                while (!_parentMap->isGuard(_ptr->getLeft()))
                {
                    _ptr = _ptr->getLeft();
                }
            }
            else
            {
                auto y = _ptr->getParent();
                if (_parentMap->isGuard(y))
                {
                    _ptr = y;
                }
                else
                {
                    while (_ptr == y->getRight())
                    {
                        _ptr = y;
                        y = y->getParent();
                    }
                    if (_ptr->getRight() != y)
                    {
                        _ptr = y;
                    }
                }
            }
        }

        void previous()
        {
            if (!_parentMap->isGuard(_ptr->getLeft()))
            {
                _ptr = _ptr->getLeft();
                while (!_parentMap->isGuard(_ptr->getRight()))
                {
                    _ptr = _ptr->getRight();
                }
            }
            else
            {
                auto y = _ptr->getParent();
                if (_parentMap->isGuard(y))
                {
                    _ptr = y;
                }
                else
                {
                    while (_ptr == y->getLeft())
                    {
                        _ptr = y;
                        y = y->getParent();
                    }
                    if (_ptr->getLeft() != y)
                    {
                        _ptr = y;
                    }
                }
            }
        }
    };

    template <class K, class T> class Map
    {
      private:
        friend class MapIterator<K, T, false, false>;
        friend class MapIterator<K, T, true, false>;
        friend class MapIterator<K, T, false, true>;
        friend class MapIterator<K, T, true, true>;

        using MapNodePtr = MapNode<K, T> *;
        using ConstMapNodePtr = const MapNode<K, T> *;

        using Pair = std::pair<const K, T>;

        int _guard = 0;
        MapNodePtr _guardPtr = reinterpret_cast<MapNodePtr>(&_guard);
        MapNodePtr _root = _guardPtr;
        size_t _size = 0;

      public:
        using Iterator = MapIterator<K, T, false, false>;
        using ConstIterator = MapIterator<K, T, true, false>;

        using ReverseIterator = MapIterator<K, T, false, true>;
        using ConstReverseIterator = MapIterator<K, T, true, true>;

        // Constructors
        Map() = default;

        template <class InputIt> Map(InputIt first, InputIt last) { insert(first, last); }

        Map(const Map<K, T> &other) { insert(other.begin(), other.end()); }

        Map(Map<K, T> &&other)
        {
            _root = other._root;
            _size = other._size;
            _guardPtr = other._guardPtr;
            other._root = other._guardPtr;
            other._size = 0;
        }

        Map(std::initializer_list<Pair> init) { insert(init); }

        ~Map() { clear(); }

        // Assign
        Map<K, T> &operator=(const Map<K, T> &other)
        {
            clear();
            insert(other.begin(), other.end());
            return *this;
        }

        Map<K, T> &operator=(Map<K, T> &&other)
        {
            clear();
            _root = other._root;
            _size = other._size;
            _guardPtr = other._guardPtr;
            other._root = other._guardPtr;
            other._size = 0;
            return *this;
        }

        Map<K, T> &operator=(std::initializer_list<Pair> ilist)
        {
            clear();
            insert(ilist);
            return *this;
        }

        // Element access
        T &at(const K &key)
        {
            auto node = findNode(key);
            assertNode(node);
            return node->getItem();
        }

        const T &at(const K &key) const
        {
            auto node = findConstNode(key);
            assertNode(node);
            return node->getItem();
        }

        T &operator[](const K &key) { return at(key); }

        const T &operator[](const K &key) const { return at(key); }

        // Modifiers
        std::pair<Iterator, bool> insert(const Pair &value) { return insertNode(makeNode(value)); }
        std::pair<Iterator, bool> insert(Pair &&value) { return insertNode(makeNode(value)); }

        template <class InputIt> void insert(InputIt first, InputIt last)
        {
            for (auto it = first; it != last; ++it)
            {
                insertNode(makeNode(*it));
            }
        }

        void insert(const std::initializer_list<Pair> &ilist)
        {
            auto end = ilist.end();
            for (auto it = ilist.begin(); it != end; ++it)
            {
                insertNode(makeNode(*it));
            }
        }

        // template <class... Args1, class... Args2> void emplace(std::tuple<Args1...> args1, std::tuple<Args2...>
        // args2)
        // {
        //     insertNode(makeNodeWithItem(args1, args2));
        // }

        void remove(const K &key)
        {
            auto node = findNode(key);
            assertNode(node);
            removeNode(node);
        }

        void swap(Map<K, T> &other)
        {
            auto tmp{std::move(*this)};
            *this = std::move(other);
            other = std::move(tmp);
        }

        void clear()
        {
            removeAllNodes(_root);
            _root = _guardPtr;
            _size = 0;
        }

        // LookUp
        Iterator find(const K &key) { return Iterator{findNode(key)}; }

        bool contains(const K &key) { return !isGuard(findNode(key)); }

        // Capacity
        size_t size() const { return _size; }

        bool empty() const { return size() == 0; }

        // Iterators
        Iterator begin() { return Iterator{this, minimum(_root)}; }
        Iterator end() { return Iterator{this, _guardPtr}; }

        ConstIterator begin() const { return ConstIterator{this, const_cast<MapNodePtr>(minimum(_root))}; }
        ConstIterator end() const { return ConstIterator{this, const_cast<MapNodePtr>(_guardPtr)}; }

        ConstIterator cBegin() const { return ConstIterator{this, minimum(const_cast<MapNodePtr>(_root))}; }
        ConstIterator cEnd() const { return ConstIterator{this, const_cast<MapNodePtr>(_guardPtr)}; }

        ReverseIterator rBegin() { return ReverseIterator{this, maximum(_root)}; }
        ReverseIterator rEnd() { return ReverseIterator{this, _guardPtr}; }

        ConstReverseIterator rBegin() const
        {
            return ConstReverseIterator{this, const_cast<MapNodePtr>(maximum(_root))};
        }
        ConstReverseIterator rEnd() const { return ConstReverseIterator{this, const_cast<MapNodePtr>(_guardPtr)}; }

        ConstReverseIterator crBegin() const
        {
            return ConstReverseIterator{this, const_cast<MapNodePtr>(maximum(_root))};
        }
        ConstReverseIterator crEnd() const { return ConstReverseIterator{this, const_cast<MapNodePtr>(_guardPtr)}; }

      private:
        MapNodePtr findNode(const K &key) { return const_cast<MapNodePtr>(findConstNode(key)); }

        ConstMapNodePtr findConstNode(const K &key) const
        {
            auto ptr = _root;
            while (!isGuard(ptr))
            {
                if (key < ptr->getKey())
                {
                    ptr = ptr->getLeft();
                }
                else if (key > ptr->getKey())
                {
                    ptr = ptr->getRight();
                }
                else
                {
                    return ptr;
                }
            }
            return _guardPtr;
        }

        MapNodePtr minimum(MapNodePtr ptr) const
        {
            if (!isGuard(ptr))
            {
                while (!isGuard(ptr->getLeft()))
                {
                    ptr = ptr->getLeft();
                }
            }
            return ptr;
        }

        MapNodePtr maximum(MapNodePtr ptr) const
        {
            if (!isGuard(ptr))
            {
                while (!isGuard(ptr->getRight()))
                {
                    ptr = ptr->getRight();
                }
            }
            return ptr;
        }

        MapNodePtr succesor(MapNodePtr ptr)
        {
            MapNodePtr r;

            if (!isGuard(ptr))
            {
                if (!isGuard(ptr->getRight()))
                    return minimum(ptr->getRight());
                else
                {
                    r = ptr->getParent();
                    while (!isGuard(r) && (ptr == r->getRight()))
                    {
                        ptr = r;
                        r = r->getParent();
                    }
                    return r;
                }
            }
            return _guardPtr;
        }

        void rotateLeft(MapNodePtr A)
        {
            MapNodePtr B, p;

            B = A->getRight();
            if (!isGuard(B))
            {
                p = A->getParent();
                A->setRight(B->getLeft());
                if (!isGuard(A->getRight()))
                {
                    A->getRight()->setParent(A);
                }

                B->setLeft(A);
                B->setParent(p);
                A->setParent(B);

                if (!isGuard(p))
                {
                    if (p->getLeft() == A)
                    {

                        p->setLeft(B);
                    }
                    else
                    {

                        p->setRight(B);
                    }
                }
                else
                {

                    _root = B;
                }
            }
        }

        void rotateRight(MapNodePtr A)
        {
            MapNodePtr B, p;

            B = A->getLeft();
            if (!isGuard(B))
            {
                p = A->getParent();
                A->setLeft(B->getRight());
                if (!isGuard(A->getLeft()))
                {
                    A->getLeft()->setParent(A);
                }

                B->setRight(A);
                B->setParent(p);
                A->setParent(B);

                if (!isGuard(p))
                {
                    if (p->getLeft() == A)
                    {

                        p->setLeft(B);
                    }
                    else
                    {

                        p->setRight(B);
                    }
                }
                else
                {

                    _root = B;
                }
            }
        }

        std::pair<Iterator, bool> insertNode(MapNodePtr node)
        {
            MapNodePtr Y;

            auto &key = node->getKey();
            node->setLeft(_guardPtr);
            node->setRight(_guardPtr);
            node->setParent(_root);

            if (isGuard(node->getParent()))
                _root = node;
            else
                while (true)
                {
                    if (key < node->getParent()->getKey())
                    {
                        if (isGuard(node->getParent()->getLeft()))
                        {
                            node->getParent()->setLeft(node);
                            break;
                        }
                        node->setParent(node->getParent()->getLeft());
                    }
                    else if (key > node->getParent()->getKey())
                    {
                        if (isGuard(node->getParent()->getRight()))
                        {
                            node->getParent()->setRight(node);
                            break;
                        }
                        node->setParent(node->getParent()->getRight());
                    }
                    else
                    {

                        std::pair<Iterator, bool> res = {Iterator{this, node->getParent()}, false};
                        deleteNode(node);
                        return res;
                    }
                }
            node->setColor(Color::Red);
            while ((node != _root) && (node->getParent()->getColor() == Color::Red))
            {
                if (node->getParent() == node->getParent()->getParent()->getLeft())
                {
                    Y = node->getParent()->getParent()->getRight();

                    if (Y->getColor() == Color::Red)
                    {
                        node->getParent()->setColor(Color::Black);
                        Y->setColor(Color::Black);
                        node->getParent()->getParent()->setColor(Color::Red);
                        node = node->getParent()->getParent();
                        continue;
                    }

                    if (node == node->getParent()->getRight())
                    {
                        node = node->getParent();
                        rotateLeft(node);
                    }

                    node->getParent()->setColor(Color::Black);
                    node->getParent()->getParent()->setColor(Color::Red);

                    rotateRight(node->getParent()->getParent());
                    break;
                }
                else
                {
                    Y = node->getParent()->getParent()->getLeft();

                    if (Y->getColor() == Color::Red)
                    {

                        node->getParent()->setColor(Color::Black);
                        Y->setColor(Color::Black);
                        node->getParent()->getParent()->setColor(Color::Red);

                        node = node->getParent()->getParent();
                        continue;
                    }

                    if (node == node->getParent()->getLeft())
                    {
                        node = node->getParent();
                        rotateRight(node);
                    }
                    node->getParent()->setColor(Color::Black);
                    node->getParent()->getParent()->setColor(Color::Red);

                    rotateLeft(node->getParent()->getParent());
                    break;
                }
            }
            _root->setColor(Color::Black);
            ++_size;
            return {Iterator{this, node}, true};
        }

        void removeNode(MapNodePtr node)
        {
            MapNodePtr W, Y, Z;

            if (isGuard(node->getLeft()) || isGuard(node->getRight()))
            {
                Y = node;
            }
            else
            {
                Y = succesor(node);
            }

            if (!isGuard(Y->getLeft()))
            {
                Z = Y->getLeft();
            }
            else
            {
                Z = Y->getRight();
            }

            Z->setParent(Y->getParent());

            if (isGuard(Y->getParent()))
            {
                _root = Z;
            }
            else if (Y == Y->getParent()->getLeft())
            {
                Y->getParent()->setLeft(Z);
            }
            else
            {
                Y->getParent()->setRight(Z);
            }

            if (Y != node)
            {
                auto color = node->getColor();
                auto left = node->getLeft();
                auto right = node->getRight();
                auto parent = node->getParent();

                node->setColor(Y->getColor());
                node->setParent(Y->getParent());
                node->setLeft(Y->getLeft());
                node->setRight(Y->getRight());

                Y->setColor(color);
                Y->setParent(parent);
                Y->setLeft(left);
                Y->setRight(right);

                Y = node;
            }

            if (Y->getColor() == Color::Black)
                while ((Z != _root) && (Z->getColor() == Color::Black))
                    if (Z == Z->getParent()->getLeft())
                    {
                        W = Z->getParent()->getRight();

                        if (W->getColor() == Color::Red)
                        {
                            W->setColor(Color::Black);
                            Z->getParent()->setColor(Color::Red);
                            rotateLeft(Z->getParent());
                            W = Z->getParent()->getRight();
                        }

                        if ((W->getLeft()->getColor() == Color::Black) && (W->getRight()->getColor() == Color::Black))
                        {
                            W->setColor(Color::Red);
                            Z = Z->getParent();
                            continue;
                        }

                        if (W->getRight()->getColor() == Color::Black)
                        { // Przypadek 3
                            W->getLeft()->setColor(Color::Black);
                            W->setColor(Color::Red);
                            rotateRight(W);
                            W = Z->getParent()->getRight();
                        }

                        W->setColor(Z->getParent()->getColor());
                        Z->getParent()->setColor(Color::Black);
                        W->getRight()->setColor(Color::Black);
                        rotateLeft(Z->getParent());
                        Z = _root;
                    }
                    else
                    {
                        W = Z->getParent()->getLeft();

                        if (W->getColor() == Color::Red)
                        {
                            W->setColor(Color::Black);
                            Z->getParent()->setColor(Color::Red);
                            rotateRight(Z->getParent());
                            W = Z->getParent()->getLeft();
                        }

                        if ((W->getLeft()->getColor() == Color::Black) && (W->getRight()->getColor() == Color::Black))
                        {
                            W->setColor(Color::Red);
                            Z = Z->getParent();
                            continue;
                        }

                        if (W->getLeft()->getColor() == Color::Red)
                        {
                            W->getRight()->setColor(Color::Black);
                            W->setColor(Color::Red);
                            rotateLeft(W);
                            W = Z->getParent()->getLeft();
                        }

                        W->setColor(Z->getParent()->getColor());
                        Z->getParent()->setColor(Color::Black);
                        W->getLeft()->setColor(Color::Black);
                        rotateRight(Z->getParent());
                        Z = _root;
                    }

            Z->setColor(Color::Black);

            deleteNode(Y);
            --_size;
        }

        void removeAllNodes(MapNodePtr ptr)
        {
            if (!isGuard(ptr))
            {
                removeAllNodes(ptr->getLeft());
                removeAllNodes(ptr->getRight());
                deleteNode(ptr);
            }
        }

        void assertNode(ConstMapNodePtr ptr) const
        {
            if (isGuard(ptr))
            {
                throw std::out_of_range("Item was not found");
            }
        }

        void assertEmpty() const
        {
            if (empty())
            {
                throw std::runtime_error("List is empty");
            }
        }

        bool isGuard(ConstMapNodePtr const ptr) const { return ptr == _guardPtr; }

        MapNodePtr makeNode(const K &key, const T &item) { return new MapNode<K, T>(key, item); }

        MapNodePtr makeNode(K &&key, T &&item) { return new MapNode<K, T>(std::move(key), std::move(item)); }

        MapNodePtr makeNode(Pair &&pair) { return new MapNode<K, T>(std::move(pair)); }

        MapNodePtr makeNode(const Pair &pair) { return new MapNode<K, T>(pair); }

        // template <class... Args1, class... Args2>
        // MapNodePtr makeNodeWithItem(std::tuple<Args1...> args1, std::tuple<Args2...> args2)
        // {
        //     return new MapNode<K, T>(args1, args2);
        // }

        void deleteNode(MapNodePtr ptr) { delete ptr; }
    };

    template <class K, class T> bool operator==(const Map<K, T> &lhs, const Map<K, T> &rhs)
    {
        return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template <class K, class T> bool operator!=(const Map<K, T> &lhs, const Map<K, T> &rhs) { return !(lhs == rhs); }

    template <class K, class T> bool operator<(const Map<K, T> &lhs, const Map<K, T> &rhs)
    {
        return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template <class K, class T> bool operator<=(const Map<K, T> &lhs, const Map<K, T> &rhs)
    {
        return lhs < rhs || lhs == rhs;
    }

    template <class K, class T> bool operator>(const Map<K, T> &lhs, const Map<K, T> &rhs)
    {
        return std::lexicographical_compare(rhs.begin(), rhs.end(), lhs.begin(), lhs.end());
    }

    template <class K, class T> bool operator>=(const Map<K, T> &lhs, const Map<K, T> &rhs)
    {
        return lhs > rhs || lhs == rhs;
    }

    void mapMain();
} // namespace sd