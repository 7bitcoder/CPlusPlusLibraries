#include <iostream>
#include <thread>
#include <gtest/gtest.h>

#include "LinkedList.hpp"

class LinkedListTest : public ::testing::Test
{
protected:
    static void SetUpTestSuite() {}

    LinkedListTest() {}

    void SetUp() override {}

    void TearDown() override {}

    ~LinkedListTest() {}

    static void TearDownTestSuite() {}
};

struct TestClass
{
    int field;
    void method() {}
};

bool operator==(const TestClass &cl1, const TestClass &cl2) { return cl1.field == cl2.field; }
bool operator!=(const TestClass &cl1, const TestClass &cl2) { return cl1.field != cl2.field; }
bool operator<(const TestClass &cl1, const TestClass &cl2) { return cl1.field < cl2.field; }
bool operator>(const TestClass &cl1, const TestClass &cl2) { return cl1.field > cl2.field; }
bool operator<=(const TestClass &cl1, const TestClass &cl2) { return cl1.field <= cl2.field; }
bool operator>=(const TestClass &cl1, const TestClass &cl2) { return cl1.field >= cl2.field; }

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
    EXPECT_EQ(l.end(), l.end());
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
    EXPECT_EQ(l.rEnd(), l.rEnd());
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
    EXPECT_EQ(l.cEnd(), l.cEnd());
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
    EXPECT_EQ(l.crEnd(), l.crEnd());
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

TEST_F(LinkedListTest, EqualsClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};
    List<TestClass> l2 = {{1}, {2}, {3}, {4}, {5}};

    EXPECT_EQ(l, l2);
}

TEST_F(LinkedListTest, EqualsClassTest2)
{
    List<TestClass> l;
    List<TestClass> l2;

    EXPECT_EQ(l, l2);
}

TEST_F(LinkedListTest, NotEqualsClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};
    List<TestClass> l2 = {{11}, {2}, {3}, {4}, {5}};

    EXPECT_NE(l, l2);
}

TEST_F(LinkedListTest, NotEqualsClassTest2)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}, {0}};
    List<TestClass> l2 = {{11}, {2}, {3}, {4}, {5}};

    EXPECT_NE(l, l2);
}

TEST_F(LinkedListTest, LessClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};
    List<TestClass> l2 = {{1}, {2}, {3}, {4}, {6}};

    EXPECT_LT(l, l2);
}

TEST_F(LinkedListTest, LessClassTest2)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};
    List<TestClass> l2 = {{1}, {2}, {3}, {4}, {5}, {6}};

    EXPECT_LT(l, l2);
}

TEST_F(LinkedListTest, GreatherClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {6}};
    List<TestClass> l2 = {{1}, {2}, {3}, {4}, {5}};

    EXPECT_GT(l, l2);
}

TEST_F(LinkedListTest, GreatherClassTest2)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}, {6}};
    List<TestClass> l2 = {{1}, {2}, {3}, {4}, {5}};

    EXPECT_GT(l, l2);
}

TEST_F(LinkedListTest, LessOrEqualClassTest)
{
    List<TestClass> l = {{0}, {2}, {3}, {4}, {5}};
    List<TestClass> l2 = {{1}, {2}, {3}, {4}, {5}};

    EXPECT_LE(l, l2);
}

TEST_F(LinkedListTest, LessOrEqualClassTest2)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};
    List<TestClass> l2 = {{1}, {2}, {3}, {4}, {5}};

    EXPECT_LE(l, l2);
}

TEST_F(LinkedListTest, GreatherOrEqualClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};
    List<TestClass> l2 = {{0}, {2}, {3}, {4}, {5}};

    EXPECT_GE(l, l2);
}

TEST_F(LinkedListTest, GreatherOrEqualClassTest2)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};
    List<TestClass> l2 = {{1}, {2}, {3}, {4}, {5}};

    EXPECT_GE(l, l2);
}

TEST_F(LinkedListTest, PushBackClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    l.pushBack({22});
    l.pushBack({23});
    l.pushBack({23});

    EXPECT_EQ(l[0], TestClass{1});
    EXPECT_EQ(l[1], TestClass{2});
    EXPECT_EQ(l[2], TestClass{3});
    EXPECT_EQ(l[3], TestClass{4});
    EXPECT_EQ(l[4], TestClass{5});
    EXPECT_EQ(l[5], TestClass{22});
    EXPECT_EQ(l[6], TestClass{23});
    EXPECT_EQ(l[7], TestClass{23});
}

TEST_F(LinkedListTest, PushFrontClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    l.pushFront({22});
    l.pushFront({23});
    l.pushFront({23});

    EXPECT_EQ(l[0], TestClass{23});
    EXPECT_EQ(l[1], TestClass{23});
    EXPECT_EQ(l[2], TestClass{22});
    EXPECT_EQ(l[3], TestClass{1});
    EXPECT_EQ(l[4], TestClass{2});
    EXPECT_EQ(l[5], TestClass{3});
    EXPECT_EQ(l[6], TestClass{4});
    EXPECT_EQ(l[7], TestClass{5});
}

TEST_F(LinkedListTest, EmplaceBackClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    l.emplaceBack(22);
    l.emplaceBack(23);
    l.emplaceBack(23);

    EXPECT_EQ(l[0], TestClass{1});
    EXPECT_EQ(l[1], TestClass{2});
    EXPECT_EQ(l[2], TestClass{3});
    EXPECT_EQ(l[3], TestClass{4});
    EXPECT_EQ(l[4], TestClass{5});
    EXPECT_EQ(l[5], TestClass{22});
    EXPECT_EQ(l[6], TestClass{23});
    EXPECT_EQ(l[7], TestClass{23});
}

TEST_F(LinkedListTest, EmplaceFrontClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    l.emplaceFront(22);
    l.emplaceFront(23);
    l.emplaceFront(23);

    EXPECT_EQ(l[0], TestClass{23});
    EXPECT_EQ(l[1], TestClass{23});
    EXPECT_EQ(l[2], TestClass{22});
    EXPECT_EQ(l[3], TestClass{1});
    EXPECT_EQ(l[4], TestClass{2});
    EXPECT_EQ(l[5], TestClass{3});
    EXPECT_EQ(l[6], TestClass{4});
    EXPECT_EQ(l[7], TestClass{5});
}

TEST_F(LinkedListTest, InsertClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    l.insert(3, {22});
    l.insert(3, {23});
    l.insert(3, {24});

    EXPECT_EQ(l[0], TestClass{1});
    EXPECT_EQ(l[1], TestClass{2});
    EXPECT_EQ(l[2], TestClass{3});
    EXPECT_EQ(l[3], TestClass{24});
    EXPECT_EQ(l[4], TestClass{23});
    EXPECT_EQ(l[5], TestClass{22});
    EXPECT_EQ(l[6], TestClass{4});
    EXPECT_EQ(l[7], TestClass{5});
}

TEST_F(LinkedListTest, InsertLastClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    l.insert(33, {22});
    l.insert(33, {23});
    l.insert(33, {24});

    EXPECT_EQ(l[0], TestClass{1});
    EXPECT_EQ(l[1], TestClass{2});
    EXPECT_EQ(l[2], TestClass{3});
    EXPECT_EQ(l[3], TestClass{4});
    EXPECT_EQ(l[4], TestClass{5});
    EXPECT_EQ(l[5], TestClass{22});
    EXPECT_EQ(l[6], TestClass{23});
    EXPECT_EQ(l[7], TestClass{24});
}

TEST_F(LinkedListTest, EmplaceClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    l.emplace(3, 22);
    l.emplace(3, 23);
    l.emplace(3, 24);

    EXPECT_EQ(l[0], TestClass{1});
    EXPECT_EQ(l[1], TestClass{2});
    EXPECT_EQ(l[2], TestClass{3});
    EXPECT_EQ(l[3], TestClass{24});
    EXPECT_EQ(l[4], TestClass{23});
    EXPECT_EQ(l[5], TestClass{22});
    EXPECT_EQ(l[6], TestClass{4});
    EXPECT_EQ(l[7], TestClass{5});
}

TEST_F(LinkedListTest, EmplaceLastClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    l.emplace(33, 22);
    l.emplace(33, 23);
    l.emplace(33, 24);

    EXPECT_EQ(l[0], TestClass{1});
    EXPECT_EQ(l[1], TestClass{2});
    EXPECT_EQ(l[2], TestClass{3});
    EXPECT_EQ(l[3], TestClass{4});
    EXPECT_EQ(l[4], TestClass{5});
    EXPECT_EQ(l[5], TestClass{22});
    EXPECT_EQ(l[6], TestClass{23});
    EXPECT_EQ(l[7], TestClass{24});
}

TEST_F(LinkedListTest, RemoveClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    l.remove(0);
    l.remove(2);

    EXPECT_EQ(l[0], TestClass{2});
    EXPECT_EQ(l[1], TestClass{3});
    EXPECT_EQ(l[2], TestClass{5});

    EXPECT_EQ(l[0], l.front());
    EXPECT_EQ(l[2], l.back());
}

TEST_F(LinkedListTest, RemoveFailClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    EXPECT_THROW(
        try
        {
            l.remove(22);
        } catch (const std::out_of_range &e)
        {
            // and this tests that it has the correct message
            EXPECT_STREQ("Index: 22 exceeded allowed boundaries, current list size is: 5", e.what());
            throw;
        },
        std::out_of_range);
}

TEST_F(LinkedListTest, PopFrontClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    l.popFront();
    l.popFront();

    EXPECT_EQ(l[0], l.front());

    EXPECT_EQ(l[0], TestClass{3});
    EXPECT_EQ(l[1], TestClass{4});
    EXPECT_EQ(l[2], TestClass{5});
}

TEST_F(LinkedListTest, PopFrontToEmptyClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    l.popFront();
    l.popFront();
    l.popFront();
    l.popFront();

    EXPECT_EQ(l.back(), l.front());
    EXPECT_EQ(l[0], TestClass{5});

    l.popFront();

    EXPECT_TRUE(l.empty());
}

TEST_F(LinkedListTest, PopFrontFailClassTest)
{
    List<TestClass> l;

    EXPECT_THROW(
        try
        {
            l.popFront();
        } catch (const std::runtime_error &e)
        {
            // and this tests that it has the correct message
            EXPECT_STREQ("List is empty", e.what());
            throw;
        },
        std::runtime_error);
}

TEST_F(LinkedListTest, PopBackClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    l.popBack();
    l.popBack();

    EXPECT_EQ(l[0], TestClass{1});
    EXPECT_EQ(l[1], TestClass{2});
    EXPECT_EQ(l[2], TestClass{3});

    EXPECT_EQ(l[2], l.back());
}

TEST_F(LinkedListTest, PopBackToEmptyClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    l.popBack();
    l.popBack();
    l.popBack();
    l.popBack();

    EXPECT_EQ(l.back(), l.front());
    EXPECT_EQ(l[0], TestClass{1});

    l.popBack();

    EXPECT_TRUE(l.empty());
}

TEST_F(LinkedListTest, PopBackFailClassTest)
{
    List<TestClass> l;

    EXPECT_THROW(
        try
        {
            l.popBack();
        } catch (const std::runtime_error &e)
        {
            // and this tests that it has the correct message
            EXPECT_STREQ("List is empty", e.what());
            throw;
        },
        std::runtime_error);
}

TEST_F(LinkedListTest, ClearClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    EXPECT_EQ(l.size(), 5);
    EXPECT_TRUE(l.begin());

    l.clear();

    EXPECT_EQ(l.size(), 0);
    EXPECT_FALSE(l.begin());
}

TEST_F(LinkedListTest, SizeClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    EXPECT_EQ(l.size(), 5);

    l.pushBack({22});

    EXPECT_EQ(l.size(), 6);

    l.remove(0);
    l.remove(0);

    EXPECT_EQ(l.size(), 4);
}

TEST_F(LinkedListTest, EmptyClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    EXPECT_FALSE(l.empty());

    l.clear();

    EXPECT_TRUE(l.empty());
}

TEST_F(LinkedListTest, SpawnConstructorClassTest)
{
    List<TestClass> l(3, TestClass{1});

    EXPECT_EQ(l.size(), 3);

    EXPECT_EQ(l[0], TestClass{1});
    EXPECT_EQ(l[1], TestClass{1});
    EXPECT_EQ(l[2], TestClass{1});
}

TEST_F(LinkedListTest, IteratorConstructorClassTest)
{
    std::vector<TestClass> v = {{1}, {2}, {3}, {4}, {5}};
    List<TestClass> l(v.begin(), v.end());

    EXPECT_EQ(l.size(), 5);

    EXPECT_EQ(l[0], TestClass{1});
    EXPECT_EQ(l[1], TestClass{2});
    EXPECT_EQ(l[2], TestClass{3});
    EXPECT_EQ(l[3], TestClass{4});
    EXPECT_EQ(l[4], TestClass{5});
}

TEST_F(LinkedListTest, CopyConstructorClassTest)
{
    List<TestClass> v = {{1}, {2}, {3}, {4}, {5}};
    List<TestClass> l(v);

    EXPECT_EQ(l.size(), 5);

    EXPECT_EQ(l[0], TestClass{1});
    EXPECT_EQ(l[1], TestClass{2});
    EXPECT_EQ(l[2], TestClass{3});
    EXPECT_EQ(l[3], TestClass{4});
    EXPECT_EQ(l[4], TestClass{5});
}

TEST_F(LinkedListTest, MoveConstructorClassTest)
{
    List<TestClass> v = {{1}, {2}, {3}, {4}, {5}};
    List<TestClass> l(std::move(v));

    EXPECT_TRUE(v.empty());
    EXPECT_EQ(l.size(), 5);

    EXPECT_EQ(l[0], TestClass{1});
    EXPECT_EQ(l[1], TestClass{2});
    EXPECT_EQ(l[2], TestClass{3});
    EXPECT_EQ(l[3], TestClass{4});
    EXPECT_EQ(l[4], TestClass{5});
}

TEST_F(LinkedListTest, InitializerListConstructorClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};

    EXPECT_EQ(l.size(), 5);

    EXPECT_EQ(l[0], TestClass{1});
    EXPECT_EQ(l[1], TestClass{2});
    EXPECT_EQ(l[2], TestClass{3});
    EXPECT_EQ(l[3], TestClass{4});
    EXPECT_EQ(l[4], TestClass{5});
}

TEST_F(LinkedListTest, AssignmentClassTest)
{
    List<TestClass> v = {{1}, {2}, {3}, {4}, {5}};
    List<TestClass> l;
    l = v;

    EXPECT_EQ(l.size(), 5);

    EXPECT_EQ(l[0], TestClass{1});
    EXPECT_EQ(l[1], TestClass{2});
    EXPECT_EQ(l[2], TestClass{3});
    EXPECT_EQ(l[3], TestClass{4});
    EXPECT_EQ(l[4], TestClass{5});
}

TEST_F(LinkedListTest, MoveAssignmentClassTest)
{
    List<TestClass> v = {{1}, {2}, {3}, {4}, {5}};
    List<TestClass> l;
    l = std::move(v);

    EXPECT_TRUE(v.empty());
    EXPECT_EQ(l.size(), 5);

    EXPECT_EQ(l[0], TestClass{1});
    EXPECT_EQ(l[1], TestClass{2});
    EXPECT_EQ(l[2], TestClass{3});
    EXPECT_EQ(l[3], TestClass{4});
    EXPECT_EQ(l[4], TestClass{5});
}

TEST_F(LinkedListTest, InitializerListAssignmentClassTest)
{
    List<TestClass> l;
    l = {{1}, {2}, {3}, {4}, {5}};

    EXPECT_EQ(l.size(), 5);

    EXPECT_EQ(l[0], TestClass{1});
    EXPECT_EQ(l[1], TestClass{2});
    EXPECT_EQ(l[2], TestClass{3});
    EXPECT_EQ(l[3], TestClass{4});
    EXPECT_EQ(l[4], TestClass{5});
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

TEST_F(LinkedListTest, SwapClassTest)
{
    List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};
    List<TestClass> l2 = {{11}, {12}, {13}, {14}, {15}};

    l.swap(l2);

    EXPECT_EQ(l2[0], TestClass{1});
    EXPECT_EQ(l2[1], TestClass{2});
    EXPECT_EQ(l2[2], TestClass{3});
    EXPECT_EQ(l2[3], TestClass{4});
    EXPECT_EQ(l2[4], TestClass{5});
    EXPECT_EQ(l2.size(), 5);

    EXPECT_EQ(l[0], TestClass{11});
    EXPECT_EQ(l[1], TestClass{12});
    EXPECT_EQ(l[2], TestClass{13});
    EXPECT_EQ(l[3], TestClass{14});
    EXPECT_EQ(l[4], TestClass{15});
    EXPECT_EQ(l.size(), 5);
}