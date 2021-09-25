#include <iostream>
#include <thread>
#include <gtest/gtest.h>

#include "Map.hpp"
#include "LinkedList.hpp"

namespace
{
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
}

class MapTest : public ::testing::Test
{
protected:
    static void SetUpTestSuite() {}

    MapTest() {}

    void SetUp() override {}

    void TearDown() override {}

    ~MapTest() {}

    static void TearDownTestSuite() {}
};

TEST_F(MapTest, AtTest)
{
    sd::Map<int, std::string> l = {{1, "hey"}, {2, "may"}, {3, "bay"}, {4, "yay"}, {5, "tej"}};

    EXPECT_EQ(l.at(1), "hey");
    EXPECT_EQ(l.at(2), "may");
    EXPECT_EQ(l.at(3), "bay");
    EXPECT_EQ(l.at(4), "yay");
    EXPECT_EQ(l.at(5), "tej");

    EXPECT_THROW(l.at(-2), std::out_of_range);
    EXPECT_THROW(
        try
        {
            l.at(22);
        } catch (const std::out_of_range &e)
        {
            // and this tests that it has the correct message
            EXPECT_STREQ("Item was not found", e.what());
            throw;
        },
        std::out_of_range);
}

TEST_F(MapTest, AtOperatorTest)
{
    sd::Map<int, std::string> l = {{1, "hey"}, {2, "may"}, {3, "bay"}, {4, "yay"}, {5, "tej"}};

    EXPECT_EQ(l[1], "hey");
    EXPECT_EQ(l[2], "may");
    EXPECT_EQ(l[3], "bay");
    EXPECT_EQ(l[4], "yay");
    EXPECT_EQ(l[5], "tej");

    EXPECT_THROW(l[-2], std::out_of_range);
    EXPECT_THROW(
        try
        {
            l[22];
        } catch (const std::out_of_range &e)
        {
            // and this tests that it has the correct message
            EXPECT_STREQ("Item was not found", e.what());
            throw;
        },
        std::out_of_range);
}

TEST_F(MapTest, AtOperatorClassTest)
{
    sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};

    EXPECT_EQ(l[{1}], "hey");
    EXPECT_EQ(l[{2}], "may");
    EXPECT_EQ(l[{3}], "bay");
    EXPECT_EQ(l[{4}], "yay");
    EXPECT_EQ(l[{5}], "tej");

    EXPECT_THROW(l[{12}], std::out_of_range);
    EXPECT_THROW(
        try
        {
            l[{54}];
        } catch (const std::out_of_range &e)
        {
            // and this tests that it has the correct message
            EXPECT_STREQ("Item was not found", e.what());
            throw;
        },
        std::out_of_range);
}

TEST_F(MapTest, IteratorClassTest)
{
    sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};

    auto it = l.begin();
    EXPECT_EQ(*it, (std::pair<const TestClass, std::string>{{1}, "hey"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{2}, "may"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{3}, "bay"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{4}, "yay"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{5}, "tej"}));
    EXPECT_FALSE(++it);
}

TEST_F(MapTest, IteratorBackClassTest)
{
    sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};

    auto it = l.begin();
    EXPECT_EQ(*it, (std::pair<const TestClass, std::string>{{1}, "hey"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{2}, "may"}));
    EXPECT_EQ(*--it, (std::pair<const TestClass, std::string>{{1}, "hey"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{2}, "may"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{3}, "bay"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{4}, "yay"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{5}, "tej"}));
    EXPECT_EQ(*--it, (std::pair<const TestClass, std::string>{{4}, "yay"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{5}, "tej"}));
    EXPECT_FALSE(++it);
}

TEST_F(MapTest, IteratorCompareTest)
{
    sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};

    auto it = l.begin();
    EXPECT_EQ(it, l.begin());
    EXPECT_NE(++it, l.begin());
    EXPECT_NE(++it, l.end());
    EXPECT_EQ(l.end(), l.end());
    EXPECT_EQ(l.end(), --l.begin());
}

TEST_F(MapTest, IteratorPostIncrementClassTest)
{
    sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};

    auto it = l.begin();
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{1}, "hey"}));
    EXPECT_EQ(*it, (std::pair<const TestClass, std::string>{{2}, "may"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{2}, "may"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{3}, "bay"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{4}, "yay"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{5}, "tej"}));
    EXPECT_FALSE(it);
}

TEST_F(MapTest, IteratorPostDecrementClassTest)
{
    sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};

    auto it = l.begin();
    EXPECT_EQ(*it, (std::pair<const TestClass, std::string>{{1}, "hey"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{1}, "hey"}));
    EXPECT_EQ(*(it--), (std::pair<const TestClass, std::string>{{2}, "may"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{1}, "hey"}));
    EXPECT_EQ(*it, (std::pair<const TestClass, std::string>{{2}, "may"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{2}, "may"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{3}, "bay"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{4}, "yay"}));
    EXPECT_EQ(*(it--), (std::pair<const TestClass, std::string>{{5}, "tej"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{4}, "yay"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{5}, "tej"}));
    EXPECT_FALSE(it);
}

TEST_F(MapTest, IteratorAccesorClassTest)
{
    sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};

    auto it = l.begin();
    EXPECT_EQ(it->first.field, 1);
    EXPECT_EQ((++it)->first.field, 2);
    EXPECT_EQ((++it)->first.field, 3);
    EXPECT_EQ((++it)->first.field, 4);
    EXPECT_EQ((++it)->first.field, 5);
    EXPECT_FALSE(++it);
}

TEST_F(MapTest, ReverseIteratorClassTest)
{
    sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};

    auto it = l.rBegin();
    EXPECT_EQ(*it, (std::pair<const TestClass, std::string>{{5}, "tej"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{4}, "yay"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{3}, "bay"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{2}, "may"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{1}, "hey"}));
    EXPECT_FALSE(++it);
}

TEST_F(MapTest, ReverseIteratorBackClassTest)
{
    sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};

    auto it = l.rBegin();
    EXPECT_EQ(*it, (std::pair<const TestClass, std::string>{{5}, "tej"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{4}, "yay"}));
    EXPECT_EQ(*--it, (std::pair<const TestClass, std::string>{{5}, "tej"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{4}, "yay"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{3}, "bay"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{2}, "may"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{1}, "hey"}));
    EXPECT_EQ(*--it, (std::pair<const TestClass, std::string>{{2}, "may"}));
    EXPECT_FALSE(++(++it));
}

TEST_F(MapTest, ReverseIteratorCompareTest)
{
    sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};

    auto it = l.rBegin();
    EXPECT_EQ(it, l.rBegin());
    EXPECT_NE(++it, l.rBegin());
    EXPECT_NE(++it, l.rEnd());
    EXPECT_EQ(l.rEnd(), l.rEnd());
    EXPECT_EQ(l.rEnd(), --l.rBegin());
}

TEST_F(MapTest, ReverseIteratorPostIncrementClassTest)
{
    sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};

    auto it = l.rBegin();
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{5}, "tej"}));
    EXPECT_EQ(*it, (std::pair<const TestClass, std::string>{{4}, "yay"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{4}, "yay"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{3}, "bay"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{2}, "may"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{1}, "hey"}));
    EXPECT_FALSE(it);
}

TEST_F(MapTest, ReverseIteratorPostDecrementClassTest)
{
    sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};

    auto it = l.rBegin();
    EXPECT_EQ(*it, (std::pair<const TestClass, std::string>{{5}, "tej"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{5}, "tej"}));
    EXPECT_EQ(*(it--), (std::pair<const TestClass, std::string>{{4}, "yay"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{5}, "tej"}));
    EXPECT_EQ(*it, (std::pair<const TestClass, std::string>{{4}, "yay"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{4}, "yay"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{3}, "bay"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{2}, "may"}));
    EXPECT_EQ(*(it--), (std::pair<const TestClass, std::string>{{1}, "hey"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{2}, "may"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{1}, "hey"}));
    EXPECT_FALSE(it);
}

TEST_F(MapTest, ReverseIteratorAccesorClassTest)
{
    sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};

    auto it = l.rBegin();
    EXPECT_EQ(it->first.field, 5);
    EXPECT_EQ((++it)->first.field, 4);
    EXPECT_EQ((++it)->first.field, 3);
    EXPECT_EQ((++it)->first.field, 2);
    EXPECT_EQ((++it)->first.field, 1);
    EXPECT_FALSE(++it);
}

TEST_F(MapTest, ConstIteratorClassTest)
{
    sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};

    auto it = l.cBegin();
    EXPECT_EQ(*it, (std::pair<const TestClass, std::string>{{1}, "hey"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{2}, "may"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{3}, "bay"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{4}, "yay"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{5}, "tej"}));
    EXPECT_FALSE(++it);
}

TEST_F(MapTest, ConstIteratorBackClassTest)
{
    sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};

    auto it = l.cBegin();
    EXPECT_EQ(*it, (std::pair<const TestClass, std::string>{{1}, "hey"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{2}, "may"}));
    EXPECT_EQ(*--it, (std::pair<const TestClass, std::string>{{1}, "hey"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{2}, "may"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{3}, "bay"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{4}, "yay"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{5}, "tej"}));
    EXPECT_EQ(*--it, (std::pair<const TestClass, std::string>{{4}, "yay"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{5}, "tej"}));
    EXPECT_FALSE(++it);
}

TEST_F(MapTest, ConstIteratorCompareTest)
{
    sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};

    auto it = l.cBegin();
    EXPECT_EQ(it, l.cBegin());
    EXPECT_NE(++it, l.cBegin());
    EXPECT_NE(++it, l.cEnd());
    EXPECT_EQ(l.cEnd(), l.cEnd());
    EXPECT_EQ(l.cEnd(), --l.cBegin());
}

TEST_F(MapTest, ConstIteratorPostIncrementClassTest)
{
    sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};

    auto it = l.cBegin();
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{1}, "hey"}));
    EXPECT_EQ(*it, (std::pair<const TestClass, std::string>{{2}, "may"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{2}, "may"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{3}, "bay"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{4}, "yay"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{5}, "tej"}));
    EXPECT_FALSE(it);
}

TEST_F(MapTest, ConstIteratorPostDecrementClassTest)
{
    sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};

    auto it = l.cBegin();
    EXPECT_EQ(*it, (std::pair<const TestClass, std::string>{{1}, "hey"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{1}, "hey"}));
    EXPECT_EQ(*(it--), (std::pair<const TestClass, std::string>{{2}, "may"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{1}, "hey"}));
    EXPECT_EQ(*it, (std::pair<const TestClass, std::string>{{2}, "may"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{2}, "may"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{3}, "bay"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{4}, "yay"}));
    EXPECT_EQ(*(it--), (std::pair<const TestClass, std::string>{{5}, "tej"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{4}, "yay"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{5}, "tej"}));
    EXPECT_FALSE(it);
}

TEST_F(MapTest, ConstIteratorAccesorClassTest)
{
    sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};

    auto it = l.cBegin();
    EXPECT_EQ(it->first.field, 1);
    EXPECT_EQ((++it)->first.field, 2);
    EXPECT_EQ((++it)->first.field, 3);
    EXPECT_EQ((++it)->first.field, 4);
    EXPECT_EQ((++it)->first.field, 5);
    EXPECT_FALSE(++it);
}

TEST_F(MapTest, ConstReverseIteratorClassTest)
{
    sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};

    auto it = l.crBegin();
    EXPECT_EQ(*it, (std::pair<const TestClass, std::string>{{5}, "tej"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{4}, "yay"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{3}, "bay"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{2}, "may"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{1}, "hey"}));
    EXPECT_FALSE(++it);
}

TEST_F(MapTest, ConstReverseIteratorBackClassTest)
{
    sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};

    auto it = l.crBegin();
    EXPECT_EQ(*it, (std::pair<const TestClass, std::string>{{5}, "tej"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{4}, "yay"}));
    EXPECT_EQ(*--it, (std::pair<const TestClass, std::string>{{5}, "tej"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{4}, "yay"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{3}, "bay"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{2}, "may"}));
    EXPECT_EQ(*++it, (std::pair<const TestClass, std::string>{{1}, "hey"}));
    EXPECT_EQ(*--it, (std::pair<const TestClass, std::string>{{2}, "may"}));
    EXPECT_FALSE(++(++it));
}

TEST_F(MapTest, ConstReverseIteratorCompareTest)
{
    sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};

    auto it = l.crBegin();
    EXPECT_EQ(it, l.crBegin());
    EXPECT_NE(++it, l.crBegin());
    EXPECT_NE(++it, l.crEnd());
    EXPECT_EQ(l.crEnd(), l.crEnd());
    EXPECT_EQ(l.crEnd(), --l.crBegin());
}

TEST_F(MapTest, ConstReverseIteratorPostIncrementClassTest)
{
    sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};

    auto it = l.crBegin();
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{5}, "tej"}));
    EXPECT_EQ(*it, (std::pair<const TestClass, std::string>{{4}, "yay"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{4}, "yay"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{3}, "bay"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{2}, "may"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{1}, "hey"}));
    EXPECT_FALSE(it);
}

TEST_F(MapTest, ConstReverseIteratorPostDecrementClassTest)
{
    sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};

    auto it = l.crBegin();
    EXPECT_EQ(*it, (std::pair<const TestClass, std::string>{{5}, "tej"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{5}, "tej"}));
    EXPECT_EQ(*(it--), (std::pair<const TestClass, std::string>{{4}, "yay"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{5}, "tej"}));
    EXPECT_EQ(*it, (std::pair<const TestClass, std::string>{{4}, "yay"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{4}, "yay"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{3}, "bay"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{2}, "may"}));
    EXPECT_EQ(*(it--), (std::pair<const TestClass, std::string>{{1}, "hey"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{2}, "may"}));
    EXPECT_EQ(*(it++), (std::pair<const TestClass, std::string>{{1}, "hey"}));
    EXPECT_FALSE(it);
}

TEST_F(MapTest, ConstReverseIteratorAccesorClassTest)
{
    sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};

    auto it = l.crBegin();
    EXPECT_EQ(it->first.field, 5);
    EXPECT_EQ((++it)->first.field, 4);
    EXPECT_EQ((++it)->first.field, 3);
    EXPECT_EQ((++it)->first.field, 2);
    EXPECT_EQ((++it)->first.field, 1);
    EXPECT_FALSE(++it);
}

TEST_F(MapTest, EqualsClassTest)
{
    //sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};
    //sd::Map<TestClass, std::string> l2 = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};
    //
    //EXPECT_EQ(l, l2);
}

TEST_F(MapTest, EqualsClassTest2)
{
    sd::List<TestClass> l;
    sd::List<TestClass> l2;

    EXPECT_EQ(l, l2);
}

TEST_F(MapTest, NotEqualsClassTest)
{
    sd::List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};
    sd::List<TestClass> l2 = {{11}, {2}, {3}, {4}, {5}};

    EXPECT_NE(l, l2);
}

TEST_F(MapTest, NotEqualsClassTest2)
{
    sd::List<TestClass> l = {{1}, {2}, {3}, {4}, {5}, {0}};
    sd::List<TestClass> l2 = {{11}, {2}, {3}, {4}, {5}};

    EXPECT_NE(l, l2);
}

TEST_F(MapTest, LessClassTest)
{
    sd::List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};
    sd::List<TestClass> l2 = {{1}, {2}, {3}, {4}, {6}};

    EXPECT_LT(l, l2);
}

TEST_F(MapTest, LessClassTest2)
{
    sd::List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};
    sd::List<TestClass> l2 = {{1}, {2}, {3}, {4}, {5}, {6}};

    EXPECT_LT(l, l2);
}

TEST_F(MapTest, GreatherClassTest)
{
    sd::List<TestClass> l = {{1}, {2}, {3}, {4}, {6}};
    sd::List<TestClass> l2 = {{1}, {2}, {3}, {4}, {5}};

    EXPECT_GT(l, l2);
}

TEST_F(MapTest, GreatherClassTest2)
{
    sd::List<TestClass> l = {{1}, {2}, {3}, {4}, {5}, {6}};
    sd::List<TestClass> l2 = {{1}, {2}, {3}, {4}, {5}};

    EXPECT_GT(l, l2);
}

TEST_F(MapTest, LessOrEqualClassTest)
{
    sd::List<TestClass> l = {{0}, {2}, {3}, {4}, {5}};
    sd::List<TestClass> l2 = {{1}, {2}, {3}, {4}, {5}};

    EXPECT_LE(l, l2);
}

TEST_F(MapTest, LessOrEqualClassTest2)
{
    sd::List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};
    sd::List<TestClass> l2 = {{1}, {2}, {3}, {4}, {5}};

    EXPECT_LE(l, l2);
}

TEST_F(MapTest, GreatherOrEqualClassTest)
{
    sd::List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};
    sd::List<TestClass> l2 = {{0}, {2}, {3}, {4}, {5}};

    EXPECT_GE(l, l2);
}

TEST_F(MapTest, GreatherOrEqualClassTest2)
{
    sd::List<TestClass> l = {{1}, {2}, {3}, {4}, {5}};
    sd::List<TestClass> l2 = {{1}, {2}, {3}, {4}, {5}};

    EXPECT_GE(l, l2);
}

TEST_F(MapTest, InsertClassTest)
{
    sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};

    l.insert({{6}, "tej"});
    l.insert({{7}, "tej"});
    l.insert({{22}, "tej"});

    EXPECT_EQ(l[{1}], "hey");
    EXPECT_EQ(l[{2}], "may");
    EXPECT_EQ(l[{3}], "bay");
    EXPECT_EQ(l[{4}], "yay");
    EXPECT_EQ(l[{5}], "tej");
    EXPECT_EQ(l[{6}], "tej");
    EXPECT_EQ(l[{7}], "tej");
    EXPECT_EQ(l[{22}], "tej");
}

TEST_F(MapTest, InsertMoveClassTest)
{
    sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};

    l.insert(std::move(std::pair<const TestClass, std::string>({{6}, "tej"})));
    l.insert(std::move(std::pair<const TestClass, std::string>({{7}, "tej"})));
    l.insert(std::move(std::pair<const TestClass, std::string>({{22}, "tej"})));

    EXPECT_EQ(l[{1}], "hey");
    EXPECT_EQ(l[{2}], "may");
    EXPECT_EQ(l[{3}], "bay");
    EXPECT_EQ(l[{4}], "yay");
    EXPECT_EQ(l[{5}], "tej");
    EXPECT_EQ(l[{6}], "tej");
    EXPECT_EQ(l[{7}], "tej");
    EXPECT_EQ(l[{22}], "tej");
}

TEST_F(MapTest, InsertListClassTest)
{
    sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};

    l.insert({{{6}, "tej"}, {{7}, "tej"}, {{22}, "tej"}});

    EXPECT_EQ(l[{1}], "hey");
    EXPECT_EQ(l[{2}], "may");
    EXPECT_EQ(l[{3}], "bay");
    EXPECT_EQ(l[{4}], "yay");
    EXPECT_EQ(l[{5}], "tej");
    EXPECT_EQ(l[{6}], "tej");
    EXPECT_EQ(l[{7}], "tej");
    EXPECT_EQ(l[{22}], "tej");
}

TEST_F(MapTest, InsertRangeClassTest)
{
    sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};

    std::vector<std::pair<TestClass, std::string>> v = {{{6}, "tej"}, {{7}, "tej"}, {{22}, "tej"}};
    l.insert(v.begin(), v.end());

    EXPECT_EQ(l[{1}], "hey");
    EXPECT_EQ(l[{2}], "may");
    EXPECT_EQ(l[{3}], "bay");
    EXPECT_EQ(l[{4}], "yay");
    EXPECT_EQ(l[{5}], "tej");
    EXPECT_EQ(l[{6}], "tej");
    EXPECT_EQ(l[{7}], "tej");
    EXPECT_EQ(l[{22}], "tej");
}

TEST_F(MapTest, EmplaceClassTest)
{
    sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};

    l.emplace(6, "tej");
    l.emplace(7, "tej");
    l.emplace(22, "tej");

    EXPECT_EQ(l[{1}], "hey");
    EXPECT_EQ(l[{2}], "may");
    EXPECT_EQ(l[{3}], "bay");
    EXPECT_EQ(l[{4}], "yay");
    EXPECT_EQ(l[{5}], "tej");
    EXPECT_EQ(l[{6}], "tej");
    EXPECT_EQ(l[{7}], "tej");
    EXPECT_EQ(l[{22}], "tej");
}

TEST_F(MapTest, RemoveClassTest)
{
    //sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};
    //
    //l.remove({1});
    //l.remove({4});
    //
    //EXPECT_EQ(l[{2}], "may");
    //EXPECT_EQ(l[{3}], "bay");
    //EXPECT_EQ(l[{5}], "tej");
}

TEST_F(MapTest, RemoveFailClassTest)
{
    sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};

    EXPECT_THROW(
        try
        {
            l.remove({22});
        } catch (const std::out_of_range &e)
        {
            // and this tests that it has the correct message
            EXPECT_STREQ("Item was not found", e.what());
            throw;
        },
        std::out_of_range);
}

TEST_F(MapTest, ClearClassTest)
{
    sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};

    EXPECT_EQ(l.size(), 5);
    EXPECT_TRUE(l.begin());

    l.clear();

    EXPECT_EQ(l.size(), 0);
    EXPECT_FALSE(l.begin());
}

TEST_F(MapTest, SizeClassTest)
{
    sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};

    EXPECT_EQ(l.size(), 5);

    l.emplace(22, "111");

    EXPECT_EQ(l.size(), 6);

    l.remove({22});
    l.remove({5});

    EXPECT_EQ(l.size(), 4);
}

TEST_F(MapTest, EmptyClassTest)
{
    sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};

    EXPECT_FALSE(l.empty());

    l.clear();

    EXPECT_TRUE(l.empty());
}

TEST_F(MapTest, IteratorConstructorClassTest)
{
    std::vector<std::pair<TestClass, std::string>> v = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};
    sd::Map<TestClass, std::string> l(v.begin(), v.end());

    EXPECT_EQ(l.size(), 5);

    EXPECT_EQ(l[{1}], "hey");
    EXPECT_EQ(l[{2}], "may");
    EXPECT_EQ(l[{3}], "bay");
    EXPECT_EQ(l[{4}], "yay");
    EXPECT_EQ(l[{5}], "tej");
}

TEST_F(MapTest, CopyConstructorClassTest)
{
    sd::Map<TestClass, std::string> v = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};
    sd::Map<TestClass, std::string> l(v);

    EXPECT_EQ(l.size(), 5);

    EXPECT_EQ(l[{1}], "hey");
    EXPECT_EQ(l[{2}], "may");
    EXPECT_EQ(l[{3}], "bay");
    EXPECT_EQ(l[{4}], "yay");
    EXPECT_EQ(l[{5}], "tej");
}

TEST_F(MapTest, MoveConstructorClassTest)
{
    sd::Map<TestClass, std::string> v = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};
    sd::Map<TestClass, std::string> l(std::move(v));

    EXPECT_TRUE(v.empty());
    EXPECT_EQ(l.size(), 5);

    EXPECT_EQ(l[{1}], "hey");
    EXPECT_EQ(l[{2}], "may");
    EXPECT_EQ(l[{3}], "bay");
    EXPECT_EQ(l[{4}], "yay");
    EXPECT_EQ(l[{5}], "tej");
}

TEST_F(MapTest, InitializerListConstructorClassTest)
{
    sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};

    EXPECT_EQ(l.size(), 5);

    EXPECT_EQ(l[{1}], "hey");
    EXPECT_EQ(l[{2}], "may");
    EXPECT_EQ(l[{3}], "bay");
    EXPECT_EQ(l[{4}], "yay");
    EXPECT_EQ(l[{5}], "tej");
}

TEST_F(MapTest, AssignmentClassTest)
{
    sd::Map<TestClass, std::string> v = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};
    sd::Map<TestClass, std::string> l;
    l = v;

    EXPECT_EQ(l.size(), 5);

    EXPECT_EQ(l[{1}], "hey");
    EXPECT_EQ(l[{2}], "may");
    EXPECT_EQ(l[{3}], "bay");
    EXPECT_EQ(l[{4}], "yay");
    EXPECT_EQ(l[{5}], "tej");
}

TEST_F(MapTest, MoveAssignmentClassTest)
{
    sd::Map<TestClass, std::string> v = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};
    sd::Map<TestClass, std::string> l;
    l = std::move(v);

    EXPECT_TRUE(v.empty());
    EXPECT_EQ(l.size(), 5);

    EXPECT_EQ(l[{1}], "hey");
    EXPECT_EQ(l[{2}], "may");
    EXPECT_EQ(l[{3}], "bay");
    EXPECT_EQ(l[{4}], "yay");
    EXPECT_EQ(l[{5}], "tej");
}

TEST_F(MapTest, InitializerListAssignmentClassTest)
{
    sd::Map<TestClass, std::string> l;
    l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};

    EXPECT_EQ(l.size(), 5);

    EXPECT_EQ(l[{1}], "hey");
    EXPECT_EQ(l[{2}], "may");
    EXPECT_EQ(l[{3}], "bay");
    EXPECT_EQ(l[{4}], "yay");
    EXPECT_EQ(l[{5}], "tej");
}

TEST_F(MapTest, ConstClassTest)
{
    const sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};

    EXPECT_EQ(l.size(), 5);

    EXPECT_EQ(l[{1}], "hey");
    EXPECT_EQ(l[{2}], "may");
    EXPECT_EQ(l[{3}], "bay");
    EXPECT_EQ(l[{4}], "yay");
    EXPECT_EQ(l[{5}], "tej");
}

TEST_F(MapTest, SwapClassTest)
{
    sd::Map<TestClass, std::string> l = {{{1}, "hey"}, {{2}, "may"}, {{3}, "bay"}, {{4}, "yay"}, {{5}, "tej"}};
    sd::Map<TestClass, std::string> l2 = {{{11}, "hey"}, {{12}, "may"}, {{13}, "bay"}, {{14}, "yay"}};

    l.swap(l2);

    EXPECT_EQ(l.size(), 4);

    EXPECT_EQ(l[{11}], "hey");
    EXPECT_EQ(l[{12}], "may");
    EXPECT_EQ(l[{13}], "bay");
    EXPECT_EQ(l[{14}], "yay");

    EXPECT_EQ(l2.size(), 5);

    EXPECT_EQ(l2[{1}], "hey");
    EXPECT_EQ(l2[{2}], "may");
    EXPECT_EQ(l2[{3}], "bay");
    EXPECT_EQ(l2[{4}], "yay");
    EXPECT_EQ(l2[{5}], "tej");
}