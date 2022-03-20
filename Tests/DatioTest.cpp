#include <chrono>
#include <gtest/gtest.h>
#include <iostream>
#include <thread>

#include "Datio.hpp"

class DatioTest : public ::testing::Test
{
  protected:
    DatioTest() {}

    void SetUp() override {}

    void TearDown() override {}

    ~DatioTest() {}

    static void TearDownTestSuite() {}
};

TEST_F(DatioTest, ExampleTest)
{
    sd::Date date{2022, 3, 10};

    auto day = date.day();
    EXPECT_EQ(unsigned{day}, 10);
}

TEST_F(DatioTest, ExampleTest2)
{
    sd::Date date{2022, 3, 10};

    auto day = date.dayOfWeek();
    EXPECT_EQ(int{date.year()}, 2022);
    EXPECT_EQ(unsigned{date.month()}, 3);
    EXPECT_EQ(unsigned{date.day()}, 10);
    EXPECT_EQ(day, std::chrono::Thursday);
    EXPECT_EQ(date.hour().count(), 0);
    EXPECT_EQ(date.second().count(), 0);
    EXPECT_EQ(date.milisecond().count(), 0);
    EXPECT_EQ(date.minute().count(), 0);
}

TEST_F(DatioTest, ExampleTest3)
{
    sd::Date date{2022, 3, 10, 20, 11, 1, 1};

    auto day = date.dayOfWeek();
    EXPECT_EQ(int{date.year()}, 2022);
    EXPECT_EQ(unsigned{date.month()}, 3);
    EXPECT_EQ(unsigned{date.day()}, 10);
    EXPECT_EQ(day, std::chrono::Thursday);
    EXPECT_EQ(date.hour().count(), 20);
    EXPECT_EQ(date.minute().count(), 11);
    EXPECT_EQ(date.second().count(), 1);
    EXPECT_EQ(date.milisecond().count(), 1);
}
