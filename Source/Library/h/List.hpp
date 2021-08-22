#pragma once
#include <iostream>
#include <exception>
#include <memory>
#include <vector>
#include <format>

template <class T>
class Node
{
public:
    using ItemType = T;
    using NodePtr = Node<T> *;
    using ConstNodePtr = const Node<T> *;

private:
    T _item;
    NodePtr _next = nullptr;
    NodePtr _parent = nullptr;

public:
    Node() = delete;

    template <class... Types>
    Node(Types... args) : _item(args...) {}
    Node(const T &i) : _item(i) {}
    Node(T &&i) : _item(i) {}

    ~Node() = default;

    void setNextNode(NodePtr p)
    {
        if (_next)
        {
            _next->setParent(nullptr);
        }
        _next = p;
        if (_next)
        {
            _next->setParent(this);
        }
    }

    NodePtr getNextNode() { return _next; }

    ConstNodePtr getNextNode() const { return _next; }

    void setParent(NodePtr parent) { _parent = parent; }

    NodePtr getParentNode() { return _parent; }

    ConstNodePtr getParentNode() const { return _parent; }

    T &getItem() { return _item; }

    const T &getItem() const { return _item; }
};

template <typename T>
class ListIterator
{
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using pointer = T *;
    using reference = T &;
    using NodePtr = typename std::conditional<std::is_const<T>::value, const Node<typename std::remove_cv<T>::type> *, Node<T> *>::type;

protected:
    NodePtr _ptr = nullptr;

public:
    ListIterator(NodePtr ptr = nullptr) { _ptr = ptr; }
    ListIterator(const ListIterator<T> &rawIterator) = default;
    ~ListIterator() {}

    ListIterator<T> &operator=(const ListIterator<T> &rawIterator) = default;
    ListIterator<T> &operator=(NodePtr ptr)
    {
        _ptr = ptr;
        return (*this);
    }

    operator bool() const { return _ptr; }

    bool operator==(const ListIterator<T> &rawIterator) const { return _ptr == rawIterator._ptr; }
    bool operator!=(const ListIterator<T> &rawIterator) const { return _ptr != rawIterator._ptr; }

    ListIterator<T> &operator++()
    {
        _ptr = _ptr->getNextNode();
        return (*this);
    }

    ListIterator<T> &operator--()
    {
        _ptr = _ptr->getParentNode();
        return (*this);
    }

    ListIterator<T> operator++(int)
    {
        auto temp(*this);
        ++*this;
        return temp;
    }

    ListIterator<T> operator--(int)
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

template <typename T>
class ListReverseIterator
{
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using pointer = T *;
    using reference = T &;
    using NodePtr = typename std::conditional<std::is_const<T>::value, const Node<typename std::remove_cv<T>::type> *, Node<T> *>::type;

protected:
    NodePtr _ptr = nullptr;

public:
    ListReverseIterator(NodePtr ptr = nullptr) { _ptr = ptr; }
    ListReverseIterator(const ListReverseIterator<T> &rawIterator) = default;
    ~ListReverseIterator() {}

    ListReverseIterator<T> &operator=(const ListReverseIterator<T> &rawIterator) = default;
    ListReverseIterator<T> &operator=(NodePtr ptr)
    {
        _ptr = ptr;
        return (*this);
    }

    operator bool() const { return _ptr; }

    bool operator==(const ListReverseIterator<T> &rawIterator) const { return _ptr == rawIterator._ptr; }
    bool operator!=(const ListReverseIterator<T> &rawIterator) const { return _ptr != rawIterator._ptr; }

    ListReverseIterator<T> &operator++()
    {
        _ptr = _ptr->getParentNode();
        return (*this);
    }

    ListReverseIterator<T> &operator--()
    {
        _ptr = _ptr->getNextNode();
        return (*this);
    }

    ListReverseIterator<T> operator++(int)
    {
        auto temp(*this);
        ++*this;
        return temp;
    }

    ListReverseIterator<T> operator--(int)
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

template <class T>
class List
{
private:
    using NodePtr = Node<T> *;
    using ConstNodePtr = const Node<T> *;

    NodePtr _head = nullptr;
    NodePtr _tail = nullptr;
    size_t _size = 0;

public:
    using Iterator = ListIterator<T>;
    using ConstIterator = ListIterator<const T>;

    using ReverseIterator = ListReverseIterator<T>;
    using ConstReverseIterator = ListReverseIterator<const T>;

    // Constructors
    List() = default;

    List(size_t count, const T &value = T())
    {
        for (int i = 0; i < count; ++i)
        {
            pushBack(value);
        }
    }

    template <class InputIt>
    List(InputIt first, InputIt last)
    {
        for (InputIt it = first; it != last; ++it)
        {
            pushBack((*it));
        }
    }

    List(const List<T> &other)
    {
        auto end = other.end();
        for (auto it = other.begin(); it != end; ++it)
        {
            pushBack(*it);
        }
    }

    List(List<T> &&other)
    {
        _head = other._head;
        _tail = other._tail;
        _size = other._size;
        other._head = nullptr;
        other._tail = nullptr;
        other._size = 0;
    }

    List(std::initializer_list<T> init)
    {
        auto end = init.end();
        for (auto it = init.begin(); it != end; ++it)
        {
            pushBack(*it);
        }
    }

    ~List() { clear(); }

    // Assign
    List<T> &operator=(const List<T> &other)
    {
        clear();
        auto end = other.end();
        for (auto it = other.begin(); it != end; ++it)
        {
            pushBack(*it);
        }
        return *this;
    }

    List<T> &operator=(List<T> &&other)
    {
        clear();
        _head = other._head;
        _tail = other._tail;
        _size = other._size;
        other._head = nullptr;
        other._tail = nullptr;
        other._size = 0;
        return *this;
    }

    List<T> &operator=(std::initializer_list<T> ilist)
    {
        clear();
        auto end = ilist.end();
        for (auto it = ilist.begin(); it != end; ++it)
        {
            pushBack(*it);
        }
        return *this;
    }

    // Element access
    T &at(size_t index)
    {
        auto ptr = getNode(index);
        return ptr->getItem();
    }

    const T &at(size_t index) const
    {
        auto ptr = getConstNode(index);
        return ptr->getItem();
    }

    T &operator[](size_t index) { return at(index); }

    const T &operator[](size_t index) const { return at(index); }

    T &front()
    {
        assertEmpty();
        return _head->getItem();
    }

    const T &front() const
    {
        assertEmpty();
        return _head->getItem();
    }

    T &back()
    {
        assertEmpty();
        return _tail->getItem();
    }

    const T &back() const
    {
        assertEmpty();
        return _tail->getItem();
    }

    // Modifiers
    void pushBack(const T &item) { insertNode(size(), makeNode(item)); }

    void pushBack(T &&item) { insertNode(size(), makeNode(std::move(item))); }

    void pushFront(const T &item) { insertNode(0, makeNode(item)); }

    void pushFront(T &&item) { insertNode(0, makeNode(std::move(item))); }

    template <class... Types>
    void emplaceBack(Types... args) { insertNode(size(), makeNodeWithItem(args...)); }

    template <class... Types>
    void emplaceFront(Types... args) { insertNode(0, makeNodeWithItem(args...)); }

    template <class... Types>
    void emplace(size_t index, Types... args) { insertNode(index, makeNodeWithItem(args...)); }

    void insert(size_t index, const T &item) { insertNode(index, makeNode(item)); }

    void insert(size_t index, T &&item) { insertNode(index, makeNode(std::move(item))); }

    void remove(size_t index) { removeNode(index); }

    void popFront()
    {
        assertEmpty();
        removeNode(0);
    }

    void popBack()
    {
        assertEmpty();
        removeNode(size() - 1);
    }

    void swap(List<T> &other) {
        auto tmpHead = _head;
        auto tmpTail = _tail;
        auto tmpSize = _size;
        _head = other._head;
        _tail = other._tail;
        _size = other._size;
        other._head = tmpHead;
        other._tail = tmpTail;
        other._size = tmpSize;
    }

    void clear() { removeAllNodes(); }

    // Capacity
    size_t size() const { return _size; }

    bool empty() const { return size() == 0; }

    // Iterators
    Iterator begin() { return Iterator{_head}; }
    Iterator end() { return Iterator{}; }

    ConstIterator begin() const { return ConstIterator{const_cast<NodePtr>(_head)}; }
    ConstIterator end() const { return ConstIterator{}; }

    ConstIterator cBegin() const { return ConstIterator{const_cast<NodePtr>(_head)}; }
    ConstIterator cEnd() const { return ConstIterator{}; }

    ReverseIterator rBegin() { return ReverseIterator{_tail}; }
    ReverseIterator rEnd() { return ReverseIterator{}; }

    ConstReverseIterator rBegin() const { return ConstReverseIterator{const_cast<NodePtr>(_tail)}; }
    ConstReverseIterator rEnd() const { return ConstReverseIterator{}; }

    ConstReverseIterator crBegin() const { return ConstReverseIterator{const_cast<NodePtr>(_tail)}; }
    ConstReverseIterator crEnd() const { return ConstReverseIterator{}; }

private:
    NodePtr getNode(size_t index) { return const_cast<NodePtr>(getConstNode(index)); }

    ConstNodePtr getConstNode(size_t index) const
    {
        assertIndex(index);
        NodePtr ptr = nullptr;
        if (index <= size() / 2)
        {
            ptr = _head;
            while (index--)
            {
                ptr = ptr->getNextNode();
                assertPointner(ptr);
            }
        }
        else
        {
            ptr = _tail;
            index = size() - index - 1;
            while (index--)
            {
                ptr = ptr->getParentNode();
                assertPointner(ptr);
            }
        }
        return ptr;
    }

    void insertNode(size_t index, NodePtr node)
    {
        if (index + 1 > size())
        {
            index = size(); // push back
        }
        if (index == 0) // insert at front
        {
            NodePtr tmp = _head;
            setHead(node);
            _head->setNextNode(tmp);
            if (size() == 0)
            {
                setTail(_head);
            }
        }
        else if (index == size()) // insert at back
        {
            _tail->setNextNode(node);
            setTail(node);
        }
        else
        {
            NodePtr toMove = getNode(index);
            NodePtr previous = toMove->getParentNode();
            previous->setNextNode(node);
            node->setNextNode(toMove);
        }
        ++_size;
    }

    void removeNode(size_t index)
    {
        assertIndex(index);
        if (index == 0) // pop front
        {
            NodePtr tmp = _head->getNextNode();
            deleteNode(_head);
            setHead(tmp);
            if (size() == 1)
            {
                setTail(tmp);
            }
        }
        else if (index == size() - 1) // pop back
        {
            NodePtr tmp = _tail->getParentNode();
            deleteNode(_tail);
            setTail(tmp);
        }
        else
        {
            NodePtr next = getNode(index + 1);
            NodePtr toRemove = next->getParentNode();
            NodePtr previous = toRemove->getParentNode();
            previous->setNextNode(next);
            deleteNode(toRemove);
        }
        --_size;
    }

    void removeAllNodes()
    {
        auto index = size();
        NodePtr ptr = _head;
        while (index--)
        {
            assertPointner(ptr);
            auto tmp = ptr->getNextNode();
            deleteNode(ptr);
            ptr = tmp;
        }
        _head = nullptr;
        _tail = nullptr;
        _size = 0;
    }

    void assertIndex(size_t index) const
    {
        if (index + 1 > size())
        {
            throw std::out_of_range(std::format("Index: {} exceeded allowed boundaries, current list size is: {}", index, size()));
        }
    }

    void assertEmpty() const
    {
        if (empty())
        {
            throw std::runtime_error("List is empty");
        }
    }

    void assertPointner(NodePtr ptr) const
    {
        if (!ptr)
        {
            throw std::runtime_error("Empty pointner");
        }
    }

    void setHead(NodePtr ptr)
    {
        _head = ptr;
        if (_head)
        {
            _head->setParent(nullptr);
        }
    }

    void setTail(NodePtr ptr)
    {
        _tail = ptr;
        if (_tail)
        {
            _tail->setNextNode(nullptr);
        }
    }

    NodePtr makeNode(const T &item) { return new Node<T>(item); }

    NodePtr makeNode(T &&item) { return new Node<T>(std::move(item)); }

    template <class... Types>
    NodePtr makeNodeWithItem(Types... args) { return new Node<T>(args...); }

    void deleteNode(NodePtr ptr) { delete ptr; }
};

template <class T>
bool operator==(const List<T> &lhs, const List<T> &rhs) { return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end()); }

template <class T>
bool operator!=(const List<T> &lhs, const List<T> &rhs) { return !(lhs == rhs); }

template <class T>
bool operator<(const List<T> &lhs, const List<T> &rhs) { return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end()); }

template <class T>
bool operator<=(const List<T> &lhs, const List<T> &rhs) { return lhs < rhs || lhs == rhs; }

template <class T>
bool operator>(const List<T> &lhs, const List<T> &rhs) { return std::lexicographical_compare(rhs.begin(), rhs.end(), lhs.begin(), lhs.end()); }

template <class T>
bool operator>=(const List<T> &lhs, const List<T> &rhs) { return lhs > rhs || lhs == rhs; }

void linkedMain();