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

TEST_F(ArrayTest, CreateFromDataTest)
{
    int data[] = {1, 2, 3, 4, 5};
    array *array = array_from_data(data, 5, sizeof(int));

    EXPECT_TRUE(array);

    EXPECT_EQ(*(int *)array_at(array, 0), 1);
    EXPECT_EQ(*(int *)array_at(array, 1), 2);
    EXPECT_EQ(*(int *)array_at(array, 2), 3);
    EXPECT_EQ(*(int *)array_at(array, 3), 4);
    EXPECT_EQ(*(int *)array_at(array, 4), 5);
    EXPECT_FALSE(array_at(array, 5));

    array_destroy(array);
}

TEST_F(ArrayTest, CloneTest)
{
    int data[] = {1, 2, 3, 4, 5};
    array *arr = array_from_data(data, 5, sizeof(int));
    array *dest = array_clone(arr);
    EXPECT_TRUE(arr);

    EXPECT_EQ(*(int *)array_at(dest, 0), 1);
    EXPECT_EQ(*(int *)array_at(dest, 1), 2);
    EXPECT_EQ(*(int *)array_at(dest, 2), 3);
    EXPECT_EQ(*(int *)array_at(dest, 3), 4);
    EXPECT_EQ(*(int *)array_at(dest, 4), 5);
    EXPECT_FALSE(array_at(dest, 5));

    array_destroy(dest);
    array_destroy(arr);
}

TEST_F(ArrayTest, CopyTest)
{
    array *arr = array_create(3, sizeof(int));
    array *dest = array_create(4, sizeof(int));

    int value = 123;
    array_set_at(arr, 0, &value);
    *(int *)array_at(arr, 1) = 12;
    *(int *)array_at(arr, 2) = -12;

    EXPECT_TRUE(array_copy(dest, arr, 3));

    EXPECT_EQ(*(int *)array_at(dest, 0), 123);
    EXPECT_EQ(*(int *)array_at(dest, 1), 12);
    EXPECT_EQ(*(int *)array_at(dest, 2), -12);
    EXPECT_EQ(*(int *)array_at(dest, 3), 0);
    EXPECT_FALSE(array_at(dest, 5));

    array_destroy(arr);
    array_destroy(dest);
}

TEST_F(ArrayTest, CopyDataTest)
{
    array *arr = array_create(3, sizeof(int));

    int data[] = {1, 2, 3};

    EXPECT_TRUE(array_copy_data(arr, data, 3));

    EXPECT_EQ(*(int *)array_at(arr, 0), 1);
    EXPECT_EQ(*(int *)array_at(arr, 1), 2);
    EXPECT_EQ(*(int *)array_at(arr, 2), 3);
    EXPECT_FALSE(array_at(arr, 5));

    array_destroy(arr);
}

TEST_F(ArrayTest, ElementAtTest)
{
    array *array = array_create(3, sizeof(int));

    int value = 123;
    array_set_at(array, 0, &value);
    *(int *)array_at(array, 1) = 12;
    *(int *)array_at(array, 2) = -12;

    EXPECT_EQ(*(int *)array_at(array, 0), 123);

    array_get_at(array, 1, &value);
    EXPECT_EQ(value, 12);

    EXPECT_EQ(*(int *)array_at(array, 2), -12);
    EXPECT_FALSE(array_at(array, 5));

    array_destroy(array);
}

TEST_F(ArrayTest, FirstElementTest)
{
    array *array = array_create(3, sizeof(int));

    int value = 123;
    array_set_at(array, 0, &value);
    *(int *)array_at(array, 1) = 12;
    *(int *)array_at(array, 2) = -12;

    EXPECT_EQ(*(int *)array_front(array), 123);

    array_destroy(array);
}

TEST_F(ArrayTest, LastElementTest)
{
    array *array = array_create(3, sizeof(int));

    int value = 123;
    array_set_at(array, 0, &value);
    *(int *)array_at(array, 1) = 12;
    *(int *)array_at(array, 2) = -12;

    EXPECT_EQ(*(int *)array_back(array), -12);

    array_destroy(array);
}

TEST_F(ArrayTest, SizeTest)
{
    array *array = array_create(3, sizeof(int));

    int value = 123;
    array_set_at(array, 0, &value);
    *(int *)array_at(array, 1) = 12;
    *(int *)array_at(array, 2) = -12;

    EXPECT_EQ(array_length(array), 3);

    array_destroy(array);
}

TEST_F(ArrayTest, ItemSizeTest)
{
    array *array = array_create(3, sizeof(int));

    EXPECT_EQ(array_item_size(array), sizeof(int));

    array_destroy(array);
}
