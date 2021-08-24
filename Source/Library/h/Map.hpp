#pragma once
#include <iostream>
#include <format>

namespace sd
{
    enum Color : char
    {
        Black,
        Red
    };

    template <class K, class T>
    class MapNode
    {
    public:
        using KeyType = K;
        using ItemType = T;
        using MapNodePtr = MapNode<K, T> *;
        using ConstMapNodePtr = const MapNode<K, T> *;

    private:
        K _key;
        T _item;
        Color _color = Color::Black;
        MapNodePtr _parent = nullptr;
        MapNodePtr _left = nullptr;
        MapNodePtr _right = nullptr;

    public:
        MapNode() = delete;

        template <class... Types>
        MapNode(const K &k, Types... args) : _key(k), _item(args...) {}
        MapNode(const K &k, const T &i) : _key(k), _item(i) {}
        MapNode(K &&k, T &&i) : _key(k), _item(i) {}

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

        K &getKey() { return _key; }

        const K &getKey() const { return _key; }

        T &getItem() { return _item; }

        const T &getItem() const { return _item; }

        MapNodePtr getNextConst(const K &key, bool &found) const
        {
            found = false;
            if (getKey() < key)
            {
                return getLeft();
            }
            else if (getKey() > key)
            {
                return getRight();
            }
            else
            {
                found = true;
                return this;
            }
        }

        Color getColor() const { return _color; }

        void setColor(Color color) { _color = color; }
    };

    /*template <class K, class T, bool R> // R = Reverse
    class MapIterator
    {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using pointer = T *;
        using reference = T &;
        using MapNodePtr = std::conditional_t<std::is_const<T>::value, const MapNode<std::remove_cv_t<T>> *, MapNode<K, T> *>;

    protected:
        MapNodePtr _ptr = nullptr;

    public:
        MapIterator(MapNodePtr ptr = nullptr) { _ptr = ptr; }
        MapIterator(const MapIterator<T, R> &rawIterator) = default;
        ~MapIterator() = default;

        MapIterator<T, R> &operator=(const MapIterator<T, R> &rawIterator) = default;
        MapIterator<T, R> &operator=(MapNodePtr ptr)
        {
            _ptr = ptr;
            return (*this);
        }

        operator bool() const { return _ptr; }

        bool operator==(const MapIterator<T, R> &rawIterator) const { return _ptr == rawIterator._ptr; }
        bool operator!=(const MapIterator<T, R> &rawIterator) const { return _ptr != rawIterator._ptr; }

        MapIterator<T, R> &operator++()
        {
            if constexpr (R)
            {
                _ptr = _ptr->getLeft();
            }
            else
            {
                _ptr = _ptr->getRight();
            }
            return (*this);
        }

        MapIterator<T, R> &operator--()
        {
            if constexpr (R)
            {
                _ptr = _ptr->getRight();
            }
            else
            {
                _ptr = _ptr->getLeft();
            }
            return (*this);
        }

        MapIterator<T, R> operator++(int)
        {
            auto temp(*this);
            ++*this;
            return temp;
        }

        MapIterator<T, R> operator--(int)
        {
            auto temp(*this);
            --*this;
            return temp;
        }

        T &operator*() { return _ptr->getItem(); }
        const T &operator*() const { return _ptr->getItem(); }

        T *operator->() { return &_ptr->getItem(); }
        const T *operator->() const { return &_ptr->getItem(); }
    };
    */

    template <class K, class T>
    class Map
    {
    private:
        using MapNodePtr = MapNode<K, T> *;
        using ConstMapNodePtr = const MapNode<K, T> *;

        int _guard = 0;
        MapNodePtr _guardPtr = reinterpret_cast<MapNodePtr>(&_guard);
        MapNodePtr _root = _guardPtr;
        size_t _size = 0;

    public:
        //using Iterator = MapIterator<T, false>;
        //using ConstIterator = MapIterator<const T, false>;

        // using ReverseIterator = MapIterator<T, true>;
        //using ConstReverseIterator = MapIterator<const T, true>;

        // Constructors
        Map() = default;

        Map(size_t count, const T &value = T())
        {
            for (int i = 0; i < count; ++i)
            {
                pushBack(value);
            }
        }

        template <class InputIt>
        Map(InputIt first, InputIt last)
        {
            //for (InputIt it = first; it != last; ++it)
            //{
            //    pushBack((*it));
            //}
        }

        Map(const Map<K, T> &other)
        {
            //auto end = other.end();
            //for (auto it = other.begin(); it != end; ++it)
            //{
            //    pushBack(*it);
            //}
        }

        Map(Map<K, T> &&other)
        {
            _root = other._root;
            _size = other._size;
            other._root = nullptr;
            other._size = 0;
        }

        Map(std::initializer_list<T> init)
        {
            //auto end = init.end();
            //for (auto it = init.begin(); it != end; ++it)
            //{
            //    pushBack(*it);
            //}
        }

        ~Map() { clear(); }

        // Assign
        Map<K, T> &operator=(const Map<K, T> &other)
        {
            //clear();
            //auto end = other.end();
            //for (auto it = other.begin(); it != end; ++it)
            //{
            //    pushBack(*it);
            //}
            //return *this;
        }

        Map<K, T> &operator=(Map<K, T> &&other)
        {
            clear();
            _root = other._root;
            _size = other._size;
            other._root = nullptr;
            other._size = 0;
            return *this;
        }

        Map<K, T> &operator=(std::initializer_list<T> ilist)
        {
            //clear();
            //auto end = ilist.end();
            //for (auto it = ilist.begin(); it != end; ++it)
            //{
            //    pushBack(*it);
            //}
            //return *this;
        }

        // Element access
        T &at(const K &key)
        {
            auto ptr = findNode(key);
            return ptr->getItem();
        }

        const T &at(const K &key) const
        {
            auto ptr = findNode(key);
            return ptr->getItem();
        }

        T &operator[](const K &key) { return at(key); }

        const T &operator[](const K &key) const { return at(key); }

        // Modifiers
        void pushBack(const K &key, const T &item) { insertNode(makeNode(key, item)); }

        void pushBack(K &&key, T &&item) { insertNode(makeNode(std::move(key), std::move(item))); }

        template <class... Types>
        void emplaceBack(const K &key, Types... args) { insertNode(makeNodeWithItem(key, args...)); }

        void remove(const K &key)
        {
            auto ptr = findNode(key);
            if (ptr)
            {
                removeNode(ptr);
            }
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

        // Capacity
        size_t size() const { return _size; }

        bool empty() const { return size() == 0; }

        // Iterators
        //Iterator begin() { return Iterator{_head}; }
        //Iterator end() { return Iterator{}; }
        //
        //ConstIterator begin() const { return ConstIterator{const_cast<MapNodePtr>(_head)}; }
        //ConstIterator end() const { return ConstIterator{}; }
        //
        //ConstIterator cBegin() const { return ConstIterator{const_cast<MapNodePtr>(_head)}; }
        //ConstIterator cEnd() const { return ConstIterator{}; }
        //
        //ReverseIterator rBegin() { return ReverseIterator{_tail}; }
        //ReverseIterator rEnd() { return ReverseIterator{}; }
        //
        //ConstReverseIterator rBegin() const { return ConstReverseIterator{const_cast<MapNodePtr>(_tail)}; }
        //ConstReverseIterator rEnd() const { return ConstReverseIterator{}; }
        //
        //ConstReverseIterator crBegin() const { return ConstReverseIterator{const_cast<MapNodePtr>(_tail)}; }
        //ConstReverseIterator crEnd() const { return ConstReverseIterator{}; }

    private:
        const T &getConstItem(const K &key) const
        {
            auto node = findNode(key);
            assertNode(node);
            return node->getItem();
        }

        T &getItem(const K &key) { const_cast<T &>(getConstItem(key)); }

        MapNodePtr findNode(const K &key) { return const_cast<MapNodePtr>(findNode(key)); }

        ConstMapNodePtr findNode(const K &key) const
        {
            auto ptr = _root;
            bool found = false;
            while (!isGuard(ptr) && !found)
            {
                ptr = ptr->getNextConst(key, found);
            }
            return found ? ptr : nullptr;
        }

        MapNodePtr minimum(MapNodePtr ptr)
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

        std::pair<MapNodePtr, bool> insertNode(MapNodePtr node)
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

                        std::pair<MapNodePtr, bool> res = {node->getParent(), false};
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
                        node->getParent()
                            ->getParent()
                            ->setColor(Color::Red);

                        node = node->getParent()->getParent();
                        continue;
                    }

                    if (node == node->getParent()->getLeft())
                    {
                        node = node->getParent();
                        rotateRight(node);
                    }
                    node->getParent()->setColor(Color::Black);
                    node->getParent()
                        ->getParent()
                        ->setColor(Color::Red);

                    rotateLeft(node->getParent()->getParent());
                    break;
                }
            }
            _root->setColor(Color::Black);
            ++_size;
            return {node, true};
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
            if (!ptr)
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

        bool isGuard(ConstMapNodePtr ptr)
        {
            return ptr == _guardPtr;
        }

        MapNodePtr makeNode(const K &key, const T &item) { return new MapNode<K, T>(key, item); }

        MapNodePtr makeNode(K &&key, T &&item) { return new MapNode<K, T>(std::move(key), std::move(item)); }

        template <class... Types>
        MapNodePtr makeNodeWithItem(const K &key, Types... args) { return new MapNode<K, T>(key, args...); }

        void deleteNode(MapNodePtr ptr) { delete ptr; }
    };

    /*template <class T>
    bool operator==(const Map<K, T> &lhs, const Map<K, T> &rhs) { return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end()); }
    
    template <class T>
    bool operator!=(const Map<K, T> &lhs, const Map<K, T> &rhs) { return !(lhs == rhs); }
    
    template <class T>
    bool operator<(const Map<K, T> &lhs, const Map<K, T> &rhs) { return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end()); }
    
    template <class T>
    bool operator<=(const Map<K, T> &lhs, const Map<K, T> &rhs) { return lhs < rhs || lhs == rhs; }
    
    template <class T>
    bool operator>(const Map<K, T> &lhs, const Map<K, T> &rhs) { return std::lexicographical_compare(rhs.begin(), rhs.end(), lhs.begin(), lhs.end()); }
    
    template <class T>
    bool operator>=(const Map<K, T> &lhs, const Map<K, T> &rhs) { return lhs > rhs || lhs == rhs; }
    */
    void mapMain();
}