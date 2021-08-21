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

    NodePtr getNextNode()
    {
        return _next;
    }

    ConstNodePtr getNextNode() const
    {
        return _next;
    }

    void setParent(NodePtr parent)
    {
        _parent = parent;
    }

    NodePtr getParentNode()
    {
        return _parent;
    }

    ConstNodePtr getParentNode() const
    {
        return _parent;
    }

    T &getItem()
    {
        return _item;
    }

    const T &getItem() const
    {
        return _item;
    }
};

template <typename T>
class ListIterator
{
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using pointer = T *;
    using reference = T &;
    using Ptr = typename std::conditional<std::is_const<T>::value, const Node<typename std::remove_cv<T>::type> *, Node<T> *>::type;

protected:
    Ptr _ptr;

public:
    ListIterator(Ptr ptr = nullptr) { _ptr = ptr; }
    ListIterator(const ListIterator<T> &rawIterator) = default;
    ~ListIterator() {}

    ListIterator<T> &operator=(const ListIterator<T> &rawIterator) = default;
    ListIterator<T> &operator=(Ptr ptr)
    {
        _ptr = ptr;
        return (*this);
    }

    operator bool() const { return _ptr; }

    bool operator==(const ListIterator<T> &rawIterator) const { return *this && rawIterator && (_ptr == rawIterator._ptr); }
    bool operator!=(const ListIterator<T> &rawIterator) const { return !(*this == rawIterator); }

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
    using Ptr = typename std::conditional<std::is_const<T>::value, const Node<typename std::remove_cv<T>::type> *, Node<T> *>::type;

protected:
    Ptr _ptr;

public:
    ListReverseIterator(Ptr ptr = nullptr) { _ptr = ptr; }
    ListReverseIterator(const ListReverseIterator<T> &rawIterator) = default;
    ~ListReverseIterator() {}

    ListReverseIterator<T> &operator=(const ListReverseIterator<T> &rawIterator) = default;
    ListReverseIterator<T> &operator=(Ptr ptr)
    {
        _ptr = ptr;
        return (*this);
    }

    operator bool() const { return _ptr; }

    bool operator==(const ListReverseIterator<T> &rawIterator) const { return *this && rawIterator && (_ptr == rawIterator._ptr); }
    bool operator!=(const ListReverseIterator<T> &rawIterator) const { return !(*this == rawIterator); }

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
    typedef ListIterator<T> Iterator;
    typedef ListIterator<const T> ConstIterator;

    typedef ListReverseIterator<T> ReverseIterator;
    typedef ListReverseIterator<const T> ConstReverseIterator;

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
        for (auto it = first; it != last; ++it)
        {
            pushBack(*it);
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
        auto end = other.end();
        for (auto it = other.begin(); it != end; ++it)
        {
            pushBack(std::move(*it));
        }
        other.clear();
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
        auto end = other.end();
        for (auto it = other.begin(); it != end; ++it)
        {
            pushBack(*it);
        }
        return *this;
    }

    List<T> &operator=(List<T> &&other)
    {
        auto end = other.end();
        for (auto it = other.begin(); it != end; ++it)
        {
            pushBack(std::move(*it));
        }
        other.clear();
        return *this;
    }

    List<T> &operator=(std::initializer_list<T> ilist)
    {
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

    T &operator[](size_t index)
    {
        return at(index);
    }

    const T &operator[](size_t index) const
    {
        return at(index);
    }

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
    void pushBack(const T &item)
    {
        insertNode(size(), makeNode(item));
    }

    void pushBack(T &&item)
    {
        insertNode(size(), makeNode(std::move(item)));
    }

    template <class... Types>
    void emplaceBack(Types... args)
    {
        insertNode(size(), makeNodeWithItem(args...));
    }

    template <class... Types>
    void emplace(size_t index, Types... args)
    {
        insertNode(index, makeNodeWithItem(args...));
    }

    void insert(size_t index, const T &item)
    {
        insertNode(index, makeNode(item));
    }

    void insert(size_t index, T &&item)
    {
        insertNode(index, makeNode(std::move(item)));
    }

    void remove(size_t index)
    {
        removeNode(index);
    }

    void clear()
    {
        removeAllNodes();
    }

    // Capacity
    size_t size() const
    {
        return _size;
    }

    bool empty() const
    {
        return size() == 0;
    }

    // Iterators
    Iterator begin() { return Iterator(_head); }
    Iterator end() { return Iterator(nullptr); }

    ConstIterator begin() const { return ConstIterator(const_cast<NodePtr>(_head)); }
    ConstIterator end() const { return ConstIterator(); }

    ConstIterator cBegin() const { return ConstIterator(const_cast<NodePtr>(_head)); }
    ConstIterator cEnd() const { return ConstIterator(); }

    ReverseIterator rBegin() { return ReverseIterator(_tail); }
    ReverseIterator rEnd() { return ReverseIterator(); }

    ConstReverseIterator rBegin() const { return ConstReverseIterator(const_cast<NodePtr>(_tail)); }
    ConstReverseIterator rEnd() const { return ConstReverseIterator(); }

    ConstReverseIterator crBegin() const { return ConstReverseIterator(const_cast<NodePtr>(_tail)); }
    ConstReverseIterator crEnd() const { return ConstReverseIterator(); }

private:
    ConstNodePtr getConstNode(size_t index) const
    {
        indexGuard(index);
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

    NodePtr getNode(size_t index)
    {
        return const_cast<NodePtr>(getConstNode(index));
    }

    void removeNode(size_t index)
    {
        indexGuard(index);
        if (index == 0)
        {
            auto tmp = _head->getNextNode();
            deleteNode(_head);
            _head = tmp;
            _head->setParent(nullptr);
        }
        else
        {
            NodePtr previous = getNode(index - 1);
            if (NodePtr toRemove = previous->getNextNode())
            {
                previous->setNextNode(toRemove->getNextNode());
                deleteNode(toRemove);
            }
            else
            {
                assertPointner(nullptr);
            }
        }
        --_size;
    }

    void insertNode(size_t index, NodePtr node)
    {
        if (index + 1 > size())
        {
            index = size(); // push back
        }
        if (index == 0)
        {
            auto tmp = _head;
            _head = node;
            _head->setNextNode(tmp);
            _head->setParent(nullptr);
            if (size() == 0)
            {
                _tail = _head;
            }
        }
        else
        {
            NodePtr previous = getNode(index - 1);
            if (auto toMove = previous->getNextNode())
            {
                auto tmp = toMove;
                previous->setNextNode(node);
                node->setNextNode(tmp);
            }
            else
            {
                previous->setNextNode(node);
                _tail = node;
            }
        }
        ++_size;
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

    void indexGuard(size_t index) const
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

    NodePtr makeNode(const T &item)
    {
        return new Node<T>(item);
    }

    NodePtr makeNode(T &&item)
    {
        return new Node<T>(std::move(item));
    }

    template <class... Types>
    NodePtr makeNodeWithItem(Types... args)
    {
        return new Node<T>(args...);
    }

    void deleteNode(NodePtr ptr)
    {
        delete ptr;
    }
};
template <class T>
bool operator==(const List<T> &lhs, const List<T> &rhs)
{
    if (lhs.size() != rhs.size())
    {
        return false;
    }

    auto lIt = lhs.begin(), rIt = rhs.begin();
    auto lEnd = lhs.end(), rEnd = rhs.end();

    for (; lIt != lEnd || rIt != rEnd(); ++lIt, ++rIt)
    {
        if (*lIt == *rIt)
        {
            continue;
        }
        return false;
    }
    return true;
}

template <class T>
bool operator!=(const List<T> &lhs, const List<T> &rhs)
{
    if (lhs.size() != rhs.size())
    {
        return true;
    }

    auto lIt = lhs.begin(), rIt = rhs.begin();
    auto lEnd = lhs.end(), rEnd = rhs.end();

    for (; lIt != lEnd || rIt != rEnd(); ++lIt, ++rIt)
    {
        if (*lIt != *rIt)
        {
            continue;
        }
        return false;
    }
    return true;
}

template <class T>
bool operator<(const List<T> &lhs, const List<T> &rhs);

template <class T>
bool operator<=(const List<T> &lhs, const List<T> &rhs);

template <class T>
bool operator>(const List<T> &lhs, const List<T> &rhs);

template <class T>
bool operator>=(const List<T> &lhs, const List<T> &rhs);

void linkedMain();