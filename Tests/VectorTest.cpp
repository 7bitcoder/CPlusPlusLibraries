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

    EXPECT_EQ(vector_size(vector), 3);

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
    vector *vector = vector_create(4, sizeof(int));

    int value = 123;
    vector_set_at(vector, 0, &value);
    vector_set_at(vector, 1, &(value = 12));
    *(int *)vector_at(vector, 2) = -12;

    int element = 9;
    vector_append(vector, &element);
    element = 1;
    vector_append(vector, &element);

    EXPECT_EQ(*(int *)vector_at(vector, 0), 123);
    EXPECT_EQ(*(int *)vector_at(vector, 1), 12);
    EXPECT_EQ(*(int *)vector_at(vector, 2), -12);
    EXPECT_EQ(*(int *)vector_at(vector, 4), 9);
    EXPECT_EQ(*(int *)vector_back(vector), 1);
    EXPECT_EQ(vector_size(vector), 6);

    vector_destroy(vector);
}

TEST_F(VectorTest, PopTest)
{
    vector *vector = vector_create(2, sizeof(int));

    int value = 123;
    vector_set_at(vector, 0, &value);
    vector_set_at(vector, 1, &(value = 1));

    vector_pop(vector, &value);
    EXPECT_EQ(value, 1);
    vector_pop(vector, &value);
    EXPECT_EQ(value, 123);
    vector_pop(vector, &value);

    EXPECT_TRUE(vector_empty(vector));

    vector_destroy(vector);
}
