#include <gtest/gtest.h>
#include <iostream>
#include <thread>

#include "Array.h"

class ArrayTest : public ::testing::Test
{
  protected:
    static void SetUpTestSuite() {}

    ArrayTest() {}

    void SetUp() override {}

    void TearDown() override {}

    ~ArrayTest() {}

    static void TearDownTestSuite() {}
};

TEST_F(ArrayTest, CreateTest)
{
    array *array = array_create(19, sizeof(int));

    EXPECT_TRUE(array);

    array_destroy(array);
}

TEST_F(ArrayTest, CreateFailTest)
{
    array *array = array_create(0, sizeof(int));

    EXPECT_FALSE(array);

    array_destroy(array);
}

TEST_F(ArrayTest, CreateFail2Test)
{
    array *array = array_create(2, 0);

    EXPECT_FALSE(array);

    array_destroy(array);
}

TEST_F(ArrayTest, ElementAtTest)
{
    array *array = array_create(3, sizeof(int));

    int *first = (int *)array_at(array, 0);
    *first = 123;
    *(int *)array_at(array, 1) = 12;
    *(int *)array_at(array, 2) = -12;

    EXPECT_EQ(*(int *)array_at(array, 0), 123);
    EXPECT_EQ(*(int *)array_at(array, 1), 12);
    EXPECT_EQ(*(int *)array_at(array, 2), -12);
    EXPECT_FALSE(array_at(array, 5));

    array_destroy(array);
}

TEST_F(ArrayTest, FirstElementTest)
{
    array *array = array_create(3, sizeof(int));

    int *first = (int *)array_at(array, 0);
    *first = 123;
    *(int *)array_at(array, 1) = 12;
    *(int *)array_at(array, 2) = -12;

    EXPECT_EQ(*(int *)array_front(array), 123);

    array_destroy(array);
}

TEST_F(ArrayTest, LastElementTest)
{
    array *array = array_create(3, sizeof(int));

    int *first = (int *)array_at(array, 0);
    *first = 123;
    *(int *)array_at(array, 1) = 12;
    *(int *)array_at(array, 2) = -12;

    EXPECT_EQ(*(int *)array_back(array), -12);

    array_destroy(array);
}

TEST_F(ArrayTest, SizeTest)
{
    array *array = array_create(3, sizeof(int));

    int *first = (int *)array_at(array, 0);
    *first = 123;
    *(int *)array_at(array, 1) = 12;
    *(int *)array_at(array, 2) = -12;

    EXPECT_EQ(array_size(array), 3);

    array_destroy(array);
}

TEST_F(ArrayTest, CopyTest)
{
    array *arr = array_create(3, sizeof(int));
    array *dest = array_create(4, sizeof(int));

    int *first = (int *)array_at(arr, 0);
    *first = 123;
    *(int *)array_at(arr, 1) = 12;
    *(int *)array_at(arr, 2) = -12;

    array_copy(arr, dest);

    EXPECT_EQ(*(int *)array_at(dest, 0), 123);
    EXPECT_EQ(*(int *)array_at(dest, 1), 12);
    EXPECT_EQ(*(int *)array_at(dest, 2), -12);
    EXPECT_EQ(*(int *)array_at(dest, 3), 0);
    EXPECT_FALSE(array_at(dest, 5));

    array_destroy(arr);
    array_destroy(dest);
}