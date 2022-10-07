#include <gtest/gtest.h>
#include <iostream>
#include <thread>

#include "Vector.h"

class VectorTest : public ::testing::Test
{
  protected:
    static void SetUpTestSuite() {}

    VectorTest() {}

    void SetUp() override {}

    void TearDown() override {}

    ~VectorTest() {}

    static void TearDownTestSuite() {}
};

TEST_F(VectorTest, CreateTest)
{
    vector *vector = vector_create(19, sizeof(int));

    EXPECT_TRUE(vector);

    vector_destroy(vector);
}

TEST_F(VectorTest, CreateFromDataTest)
{
    int data[] = {1, 2, 3, 4, 5};
    vector *vector = vector_from_data(data, 5, sizeof(int));

    EXPECT_TRUE(vector);

    EXPECT_EQ(*(int *)vector_at(vector, 0), 1);
    EXPECT_EQ(*(int *)vector_at(vector, 1), 2);
    EXPECT_EQ(*(int *)vector_at(vector, 2), 3);
    EXPECT_EQ(*(int *)vector_at(vector, 3), 4);
    EXPECT_EQ(*(int *)vector_at(vector, 4), 5);
    EXPECT_FALSE(vector_at(vector, 5));

    vector_destroy(vector);
}

TEST_F(VectorTest, CreateFromArrayTest)
{
    int data[] = {1, 2, 3, 4, 5};
    array *array = array_from_data(data, 5, sizeof(int));
    vector *vector = vector_from_array(array);

    EXPECT_TRUE(vector);

    EXPECT_EQ(*(int *)vector_at(vector, 0), 1);
    EXPECT_EQ(*(int *)vector_at(vector, 1), 2);
    EXPECT_EQ(*(int *)vector_at(vector, 2), 3);
    EXPECT_EQ(*(int *)vector_at(vector, 3), 4);
    EXPECT_EQ(*(int *)vector_at(vector, 4), 5);
    EXPECT_FALSE(vector_at(vector, 5));

    array_destroy(array);
    vector_destroy(vector);
}

TEST_F(VectorTest, CopyFromArrayTest)
{
    int data[] = {1, 2, 3, 4, 5};
    array *array = array_from_data(data, 5, sizeof(int));
    vector *vector = vector_create(5, sizeof(int));

    EXPECT_TRUE(vector);
    EXPECT_TRUE(vector_copy_array(vector, array, 5));

    EXPECT_EQ(*(int *)vector_at(vector, 0), 1);
    EXPECT_EQ(*(int *)vector_at(vector, 1), 2);
    EXPECT_EQ(*(int *)vector_at(vector, 2), 3);
    EXPECT_EQ(*(int *)vector_at(vector, 3), 4);
    EXPECT_EQ(*(int *)vector_at(vector, 4), 5);
    EXPECT_FALSE(vector_at(vector, 5));

    array_destroy(array);
    vector_destroy(vector);
}

TEST_F(VectorTest, CopyFromDataTest)
{
    int data[] = {1, 2, 3, 4, 5};
    vector *vector = vector_create(5, sizeof(int));

    EXPECT_TRUE(vector);
    EXPECT_TRUE(vector_copy_data(vector, data, 5));

    EXPECT_EQ(*(int *)vector_at(vector, 0), 1);
    EXPECT_EQ(*(int *)vector_at(vector, 1), 2);
    EXPECT_EQ(*(int *)vector_at(vector, 2), 3);
    EXPECT_EQ(*(int *)vector_at(vector, 3), 4);
    EXPECT_EQ(*(int *)vector_at(vector, 4), 5);
    EXPECT_FALSE(vector_at(vector, 5));

    vector_destroy(vector);
}

TEST_F(VectorTest, CopyTest)
{
    int data[] = {1, 2, 3, 4, 5};
    vector *vec = vector_from_data(data, 5, sizeof(int));
    vector *vector2 = vector_create(5, sizeof(int));

    EXPECT_TRUE(vector_copy(vector2, vec, 5));

    EXPECT_EQ(*(int *)vector_at(vector2, 0), 1);
    EXPECT_EQ(*(int *)vector_at(vector2, 1), 2);
    EXPECT_EQ(*(int *)vector_at(vector2, 2), 3);
    EXPECT_EQ(*(int *)vector_at(vector2, 3), 4);
    EXPECT_EQ(*(int *)vector_at(vector2, 4), 5);
    EXPECT_FALSE(vector_at(vector2, 5));

    vector_destroy(vector2);
    vector_destroy(vec);
}

TEST_F(VectorTest, CreateEmptyTest)
{
    vector *vector = vector_create(0, sizeof(int));

    EXPECT_TRUE(vector);

    vector_destroy(vector);
}

TEST_F(VectorTest, CreateFail2Test)
{
    vector *vector = vector_create(2, 0);

    EXPECT_FALSE(vector);

    vector_destroy(vector);
}

TEST_F(VectorTest, ElementAtTest)
{
    vector *vector = vector_create(3, sizeof(int));

    int value = 123;
    vector_set_at(vector, 0, &value);
    vector_set_at(vector, 1, &(value = 12));
    *(int *)vector_at(vector, 2) = -12;

    EXPECT_EQ(*(int *)vector_at(vector, 0), 123);
    EXPECT_EQ(*(int *)vector_at(vector, 1), 12);
    EXPECT_EQ(*(int *)vector_at(vector, 2), -12);
    EXPECT_FALSE(vector_at(vector, 5));

    vector_destroy(vector);
}

TEST_F(VectorTest, FirstElementTest)
{
    vector *vector = vector_create(3, sizeof(int));

    int value = 123;
    vector_set_at(vector, 0, &value);
    vector_set_at(vector, 1, &(value = 123));
    *(int *)vector_at(vector, 2) = -12;

    EXPECT_EQ(*(int *)vector_front(vector), 123);

    vector_destroy(vector);
}

TEST_F(VectorTest, LastElementTest)
{
    vector *vector = vector_create(3, sizeof(int));

    int value = 123;
    vector_set_at(vector, 0, &value);
    vector_set_at(vector, 1, &(value = 12));
    *(int *)vector_at(vector, 2) = -12;

    EXPECT_EQ(*(int *)vector_back(vector), -12);

    vector_destroy(vector);
}

TEST_F(VectorTest, SizeTest)
{
    vector *vector = vector_create(3, sizeof(int));

    int value = 123;
    vector_set_at(vector, 0, &value);
    vector_set_at(vector, 1, &(value = 12));
    *(int *)vector_at(vector, 2) = -12;

    EXPECT_EQ(vector_length(vector), 3);

    vector_destroy(vector);
}

TEST_F(VectorTest, EmptyTest)
{
    vector *vector = vector_create(0, sizeof(int));

    EXPECT_TRUE(vector_empty(vector));

    vector_destroy(vector);
}

TEST_F(VectorTest, ClearTest)
{
    vector *vector = vector_create(3, sizeof(int));

    int value = 123;
    vector_set_at(vector, 0, &value);
    vector_set_at(vector, 1, &(value = 12));
    *(int *)vector_at(vector, 2) = -12;

    vector_clear(vector);

    EXPECT_TRUE(vector_empty(vector));

    vector_destroy(vector);
}

TEST_F(VectorTest, AppendTest)
{
    int data[] = {1, 2, 3, 4};
    vector *vector = vector_from_data(data, 4, sizeof(int));

    int element = 9;
    EXPECT_TRUE(vector_append(vector, &element));
    element = 1;
    EXPECT_TRUE(vector_append(vector, &element));

    EXPECT_EQ(*(int *)vector_at(vector, 0), 1);
    EXPECT_EQ(*(int *)vector_at(vector, 1), 2);
    EXPECT_EQ(*(int *)vector_at(vector, 2), 3);
    EXPECT_EQ(*(int *)vector_at(vector, 3), 4);
    EXPECT_EQ(*(int *)vector_at(vector, 4), 9);
    EXPECT_EQ(*(int *)vector_back(vector), 1);
    EXPECT_EQ(vector_length(vector), 6);

    vector_destroy(vector);
}

TEST_F(VectorTest, PopTest)
{
    vector *vector = vector_create(2, sizeof(int));

    int value = 123;
    vector_set_at(vector, 0, &value);
    vector_set_at(vector, 1, &(value = 1));

    EXPECT_TRUE(vector_pop(vector, &value));
    EXPECT_EQ(value, 1);
    EXPECT_TRUE(vector_pop(vector, &value));
    EXPECT_EQ(value, 123);
    EXPECT_FALSE(vector_pop(vector, &value));

    EXPECT_TRUE(vector_empty(vector));

    vector_destroy(vector);
}

struct tuple
{
    int id;
    const char *name;
    double value;
};

TEST_F(VectorTest, GeneralUsage)
{
    tuple data[] = {{1, "hello", 1.2}, {2, "world", 2.0}, {3, "world", 3.0}, {4, "world", 4.0},
                    {5, "world", 5.0}, {6, "world", 6.0}, {7, "world", 7.0}};

    vector *vec = vector_from_data(data, sizeof(data) / sizeof(tuple), sizeof(tuple));

    EXPECT_EQ(vector_length(vec), 7);
    EXPECT_EQ(vector_item_size(vec), 24);
    EXPECT_FALSE(vector_empty(vec));

    tuple last, first, second, third;
    tuple *thirdPtr;
    vector_get_back(vec, &last);
    vector_get_front(vec, &first);
    vector_get_at(vec, 1, &second);
    thirdPtr = (tuple *)vector_at(vec, 2);

    EXPECT_EQ(first.id, 1);
    EXPECT_EQ(second.id, 2);
    EXPECT_EQ(last.id, 7);
    EXPECT_EQ(thirdPtr->id, 3);

    third = {30, "new", 123.0};
    vector_set_at(vec, 2, &third);
    EXPECT_EQ(thirdPtr->id, 30);

    vector_append(vec, &third);
    vector_append(vec, &third);
    vector_append(vec, &third);
    vector_append(vec, &third);
    vector_append(vec, &third);
    vector_append(vec, &third);
    vector_append(vec, &third);
    vector_get_back(vec, &last);
    EXPECT_EQ(last.id, 30);

    vector_pop(vec, &last);
    vector_pop(vec, &last);
    vector_pop(vec, &last);
    vector_pop(vec, &last);
    vector_pop(vec, &last);
    vector_pop(vec, &last);
    vector_pop(vec, &last);
    EXPECT_EQ(last.id, 30);
    vector_get_back(vec, &last);
    EXPECT_EQ(last.id, 7);
}
