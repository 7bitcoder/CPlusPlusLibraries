#include <iostream>
#include <thread>
#include <gtest/gtest.h>

#include "LinkedList.hpp"

class LinkedListTest : public ::testing::Test
{
protected:
    LinkedListTest() {}

    void SetUp() override
    {
    }

    void TearDown() override {}

    ~LinkedListTest() {}

    template<class T>

    bool equals(const List<T>& list, const std::vector<T> expect) {
        return std::equal(list.begin(), list.end(), expect.begin(), expect.end());
    }

    static void TearDownTestSuite() {}
};

TEST_F(LinkedListTest, AtTest)
{
    List<int> l = {1, 2, 3, 4, 5};

    EXPECT_EQ(l.at(0), 1);
    EXPECT_EQ(l.at(1), 2);
    EXPECT_EQ(l.at(2), 3);
    EXPECT_EQ(l.at(3), 4);
    EXPECT_EQ(l.at(4), 5);

    EXPECT_THROW(l.at(-2), std::out_of_range);
    EXPECT_THROW(
        try
        {
            l.at(22);
        } catch (const std::out_of_range &e)
        {
            // and this tests that it has the correct message
            EXPECT_STREQ("Index: 22 exceeded allowed boundaries, current list size is: 5", e.what());
            throw;
        },
        std::out_of_range);
}

TEST_F(LinkedListTest, AtOperatorTest)
{
    List<int> l = {1, 2, 3, 4, 5};

    EXPECT_EQ(l[0], 1);
    EXPECT_EQ(l[1], 2);
    EXPECT_EQ(l[2], 3);
    EXPECT_EQ(l[3], 4);
    EXPECT_EQ(l[4], 5);

    EXPECT_THROW(l[-2], std::out_of_range);
    EXPECT_THROW(
        try
        {
            l[22];
        } catch (const std::out_of_range &e)
        {
            // and this tests that it has the correct message
            EXPECT_STREQ("Index: 22 exceeded allowed boundaries, current list size is: 5", e.what());
            throw;
        },
        std::out_of_range);
}

struct TestClass
{
    int field;
    void method() {}
};

bool operator==(const TestClass &cl1, const TestClass &cl2) { return cl1.field == cl2.field; }

TEST_F(LinkedListTest, AtOperatorClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    EXPECT_EQ(l[0], TestClass{1});
    EXPECT_EQ(l[1], TestClass{2});
    EXPECT_EQ(l[2], TestClass{3});
    EXPECT_EQ(l[3], TestClass{4});
    EXPECT_EQ(l[4], TestClass{5});

    EXPECT_THROW(l[-2], std::out_of_range);
    EXPECT_THROW(
        try
        {
            l[22];
        } catch (const std::out_of_range &e)
        {
            // and this tests that it has the correct message
            EXPECT_STREQ("Index: 22 exceeded allowed boundaries, current list size is: 5", e.what());
            throw;
        },
        std::out_of_range);
}

TEST_F(LinkedListTest, FrontClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    EXPECT_EQ(l.front(), TestClass{1});
}

TEST_F(LinkedListTest, FrontClassFailTest)
{
    List<TestClass> l;

    EXPECT_THROW(
        try
        {
            l.front();
        } catch (const std::runtime_error &e)
        {
            EXPECT_STREQ("List is empty", e.what());
            throw;
        },
        std::runtime_error);
}

TEST_F(LinkedListTest, BackClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    EXPECT_EQ(l.back(), TestClass{5});
}

TEST_F(LinkedListTest, BackClassFailTest)
{
    List<TestClass> l;

    EXPECT_THROW(
        try
        {
            l.back();
        } catch (const std::runtime_error &e)
        {
            EXPECT_STREQ("List is empty", e.what());
            throw;
        },
        std::runtime_error);
}

TEST_F(LinkedListTest, IteratorClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    auto it = l.begin();
    EXPECT_EQ(*it, TestClass{1});
    EXPECT_EQ(*++it, TestClass{2});
    EXPECT_EQ(*++it, TestClass{3});
    EXPECT_EQ(*++it, TestClass{4});
    EXPECT_EQ(*++it, TestClass{5});
    EXPECT_FALSE(++it);
}


TEST_F(LinkedListTest, IteratorBackClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    auto it = l.begin();
    EXPECT_EQ(*it, TestClass{1});
    EXPECT_EQ(*++it, TestClass{2});
    EXPECT_EQ(*--it, TestClass{1});
    EXPECT_EQ(*++it, TestClass{2});
    EXPECT_EQ(*++it, TestClass{3});
    EXPECT_EQ(*++it, TestClass{4});
    EXPECT_EQ(*++it, TestClass{5});
    EXPECT_EQ(*--it, TestClass{4});
    EXPECT_FALSE(++(++it));
}

TEST_F(LinkedListTest, IteratorCompareTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    auto it = l.begin();
    EXPECT_EQ(it, l.begin());
    EXPECT_NE(++it, l.begin());
    EXPECT_NE(++it, l.end());
    EXPECT_NE(l.end(), l.end());
}

TEST_F(LinkedListTest, IteratorPostIncrementClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    auto it = l.begin();
    EXPECT_EQ(*(it++), TestClass{1});
    EXPECT_EQ(*it, TestClass{2});
    EXPECT_EQ(*(it++), TestClass{2});
    EXPECT_EQ(*(it++), TestClass{3});
    EXPECT_EQ(*(it++), TestClass{4});
    EXPECT_EQ(*(it++), TestClass{5});
    EXPECT_FALSE(it);
}

TEST_F(LinkedListTest, IteratorPostDecrementClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    auto it = l.begin();
    EXPECT_EQ(*it, TestClass{1});
    EXPECT_EQ(*(it++), TestClass{1});
    EXPECT_EQ(*(it--), TestClass{2});
    EXPECT_EQ(*(it++), TestClass{1});
    EXPECT_EQ(*it, TestClass{2});
    EXPECT_EQ(*(it++), TestClass{2});
    EXPECT_EQ(*(it++), TestClass{3});
    EXPECT_EQ(*(it++), TestClass{4});
    EXPECT_EQ(*(it--), TestClass{5});
    EXPECT_EQ(*(it++), TestClass{4});
    EXPECT_EQ(*(it++), TestClass{5});
    EXPECT_FALSE(it);
}

TEST_F(LinkedListTest, IteratorAccesorClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    auto it = l.begin();
    EXPECT_EQ(it->field, 1);
    EXPECT_EQ((++it)->field, 2);
    EXPECT_EQ((++it)->field, 3);
    EXPECT_EQ((++it)->field, 4);
    EXPECT_EQ((++it)->field, 5);
    EXPECT_FALSE(++it);
}

TEST_F(LinkedListTest, ReverseIteratorClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    auto it = l.rBegin();
    EXPECT_EQ(*it, TestClass{5});
    EXPECT_EQ(*++it, TestClass{4});
    EXPECT_EQ(*++it, TestClass{3});
    EXPECT_EQ(*++it, TestClass{2});
    EXPECT_EQ(*++it, TestClass{1});
    EXPECT_FALSE(++it);
}

TEST_F(LinkedListTest, ReverseIteratorBackClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    auto it = l.rBegin();
    EXPECT_EQ(*it, TestClass{5});
    EXPECT_EQ(*++it, TestClass{4});
    EXPECT_EQ(*--it, TestClass{5});
    EXPECT_EQ(*++it, TestClass{4});
    EXPECT_EQ(*++it, TestClass{3});
    EXPECT_EQ(*++it, TestClass{2});
    EXPECT_EQ(*++it, TestClass{1});
    EXPECT_EQ(*--it, TestClass{2});
    EXPECT_FALSE(++(++it));
}

TEST_F(LinkedListTest, ReverseIteratorCompareTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    auto it = l.rBegin();
    EXPECT_EQ(it, l.rBegin());
    EXPECT_NE(++it, l.rBegin());
    EXPECT_NE(++it, l.rEnd());
    EXPECT_NE(l.rEnd(), l.rEnd());
}

TEST_F(LinkedListTest, ReverseIteratorPostIncrementClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    auto it = l.rBegin();
    EXPECT_EQ(*(it++), TestClass{5});
    EXPECT_EQ(*it, TestClass{4});
    EXPECT_EQ(*(it++), TestClass{4});
    EXPECT_EQ(*(it++), TestClass{3});
    EXPECT_EQ(*(it++), TestClass{2});
    EXPECT_EQ(*(it++), TestClass{1});
    EXPECT_FALSE(it);
}

TEST_F(LinkedListTest, ReverseIteratorPostDecrementClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    auto it = l.rBegin();
    EXPECT_EQ(*it, TestClass{5});
    EXPECT_EQ(*(it++), TestClass{5});
    EXPECT_EQ(*(it--), TestClass{4});
    EXPECT_EQ(*(it++), TestClass{5});
    EXPECT_EQ(*it, TestClass{4});
    EXPECT_EQ(*(it++), TestClass{4});
    EXPECT_EQ(*(it++), TestClass{3});
    EXPECT_EQ(*(it++), TestClass{2});
    EXPECT_EQ(*(it--), TestClass{1});
    EXPECT_EQ(*(it++), TestClass{2});
    EXPECT_EQ(*(it++), TestClass{1});
    EXPECT_FALSE(it);
}

TEST_F(LinkedListTest, ReverseIteratorAccesorClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    auto it = l.rBegin();
    EXPECT_EQ(it->field, 5);
    EXPECT_EQ((++it)->field, 4);
    EXPECT_EQ((++it)->field, 3);
    EXPECT_EQ((++it)->field, 2);
    EXPECT_EQ((++it)->field, 1);
    EXPECT_FALSE(++it);
}

TEST_F(LinkedListTest, ConstIteratorClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    auto it = l.cBegin();
    EXPECT_EQ(*it, TestClass{1});
    EXPECT_EQ(*++it, TestClass{2});
    EXPECT_EQ(*++it, TestClass{3});
    EXPECT_EQ(*++it, TestClass{4});
    EXPECT_EQ(*++it, TestClass{5});
    EXPECT_FALSE(++it);
}

TEST_F(LinkedListTest, ConstIteratorBackClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    auto it = l.cBegin();
    EXPECT_EQ(*it, TestClass{1});
    EXPECT_EQ(*++it, TestClass{2});
    EXPECT_EQ(*--it, TestClass{1});
    EXPECT_EQ(*++it, TestClass{2});
    EXPECT_EQ(*++it, TestClass{3});
    EXPECT_EQ(*++it, TestClass{4});
    EXPECT_EQ(*++it, TestClass{5});
    EXPECT_EQ(*--it, TestClass{4});
    EXPECT_FALSE(++(++it));
}

TEST_F(LinkedListTest, ConstIteratorCompareTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    auto it = l.cBegin();
    EXPECT_EQ(it, l.cBegin());
    EXPECT_NE(++it, l.cBegin());
    EXPECT_NE(++it, l.cEnd());
    EXPECT_NE(l.cEnd(), l.cEnd());
}

TEST_F(LinkedListTest, ConstIteratorPostIncrementClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    auto it = l.cBegin();
    EXPECT_EQ(*(it++), TestClass{1});
    EXPECT_EQ(*it, TestClass{2});
    EXPECT_EQ(*(it++), TestClass{2});
    EXPECT_EQ(*(it++), TestClass{3});
    EXPECT_EQ(*(it++), TestClass{4});
    EXPECT_EQ(*(it++), TestClass{5});
    EXPECT_FALSE(it);
}

TEST_F(LinkedListTest, ConstIteratorPostDecrementClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    auto it = l.cBegin();
    EXPECT_EQ(*it, TestClass{1});
    EXPECT_EQ(*(it++), TestClass{1});
    EXPECT_EQ(*(it--), TestClass{2});
    EXPECT_EQ(*(it++), TestClass{1});
    EXPECT_EQ(*it, TestClass{2});
    EXPECT_EQ(*(it++), TestClass{2});
    EXPECT_EQ(*(it++), TestClass{3});
    EXPECT_EQ(*(it++), TestClass{4});
    EXPECT_EQ(*(it--), TestClass{5});
    EXPECT_EQ(*(it++), TestClass{4});
    EXPECT_EQ(*(it++), TestClass{5});
    EXPECT_FALSE(it);
}

TEST_F(LinkedListTest, ConstIteratorAccesorClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    auto it = l.cBegin();
    EXPECT_EQ(it->field, 1);
    EXPECT_EQ((++it)->field, 2);
    EXPECT_EQ((++it)->field, 3);
    EXPECT_EQ((++it)->field, 4);
    EXPECT_EQ((++it)->field, 5);
    EXPECT_FALSE(++it);
}

TEST_F(LinkedListTest, ConstReverseIteratorClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    auto it = l.crBegin();
    EXPECT_EQ(*it, TestClass{5});
    EXPECT_EQ(*++it, TestClass{4});
    EXPECT_EQ(*++it, TestClass{3});
    EXPECT_EQ(*++it, TestClass{2});
    EXPECT_EQ(*++it, TestClass{1});
    EXPECT_FALSE(++it);
}

TEST_F(LinkedListTest, ConstReverseIteratorBackClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    auto it = l.crBegin();
    EXPECT_EQ(*it, TestClass{5});
    EXPECT_EQ(*++it, TestClass{4});
    EXPECT_EQ(*--it, TestClass{5});
    EXPECT_EQ(*++it, TestClass{4});
    EXPECT_EQ(*++it, TestClass{3});
    EXPECT_EQ(*++it, TestClass{2});
    EXPECT_EQ(*++it, TestClass{1});
    EXPECT_EQ(*--it, TestClass{2});
    EXPECT_FALSE(++(++it));
}

TEST_F(LinkedListTest, ConstReverseIteratorCompareTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    auto it = l.crBegin();
    EXPECT_EQ(it, l.crBegin());
    EXPECT_NE(++it, l.crBegin());
    EXPECT_NE(++it, l.crEnd());
    EXPECT_NE(l.crEnd(), l.crEnd());
}

TEST_F(LinkedListTest, ConstReverseIteratorPostIncrementClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    auto it = l.crBegin();
    EXPECT_EQ(*(it++), TestClass{5});
    EXPECT_EQ(*it, TestClass{4});
    EXPECT_EQ(*(it++), TestClass{4});
    EXPECT_EQ(*(it++), TestClass{3});
    EXPECT_EQ(*(it++), TestClass{2});
    EXPECT_EQ(*(it++), TestClass{1});
    EXPECT_FALSE(it);
}

TEST_F(LinkedListTest, ConstReverseIteratorPostDecrementClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    auto it = l.crBegin();
    EXPECT_EQ(*it, TestClass{5});
    EXPECT_EQ(*(it++), TestClass{5});
    EXPECT_EQ(*(it--), TestClass{4});
    EXPECT_EQ(*(it++), TestClass{5});
    EXPECT_EQ(*it, TestClass{4});
    EXPECT_EQ(*(it++), TestClass{4});
    EXPECT_EQ(*(it++), TestClass{3});
    EXPECT_EQ(*(it++), TestClass{2});
    EXPECT_EQ(*(it--), TestClass{1});
    EXPECT_EQ(*(it++), TestClass{2});
    EXPECT_EQ(*(it++), TestClass{1});
    EXPECT_FALSE(it);
}

TEST_F(LinkedListTest, ConstReverseIteratorAccesorClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    auto it = l.crBegin();
    EXPECT_EQ(it->field, 5);
    EXPECT_EQ((++it)->field, 4);
    EXPECT_EQ((++it)->field, 3);
    EXPECT_EQ((++it)->field, 2);
    EXPECT_EQ((++it)->field, 1);
    EXPECT_FALSE(++it);
}

TEST_F(LinkedListTest, PushBackClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    l.pushBack({22});
    l.pushBack({23});
    l.pushBack({23});

    EXPECT_EQ(l.back(), TestClass{23});
}

TEST_F(LinkedListTest, ConstClassTest)
{
    const List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    EXPECT_EQ(l[0], TestClass{1});
    EXPECT_EQ(l[1], TestClass{2});
    EXPECT_EQ(l[2], TestClass{3});
    EXPECT_EQ(l[3], TestClass{4});
    EXPECT_EQ(l[4], TestClass{5});
}