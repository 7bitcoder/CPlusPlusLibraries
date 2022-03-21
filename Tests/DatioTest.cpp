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

TEST_F(DatioTest, ExampleTest4)
{
    sd::Date date{2022, 3, 10, 20, 11, 1, 1};

    date.addDays(21);
    date.addHours(6);
    date.addMinutes(2);
    date.addSeconds(62);
    date.addMiliseconds(2);
    date.addMonths(1);
    date.addYears(12);

    auto day = date.dayOfWeek();
    EXPECT_EQ(int{date.year()}, 2034);
    EXPECT_EQ(unsigned{date.month()}, 5);
    EXPECT_EQ(unsigned{date.day()}, 1);
    // EXPECT_EQ(day, std::chrono::Thursday);
    EXPECT_EQ(date.hour().count(), 2);
    EXPECT_EQ(date.minute().count(), 14);
    EXPECT_EQ(date.second().count(), 3);
    EXPECT_EQ(date.milisecond().count(), 3);
}

TEST_F(DatioTest, ExampleTest5)
{
    using namespace std::chrono;
    sd::Date date{2022, 3, 10, 20, 11, 1, 1};

    date += 21d;
    date += 6h;
    date.addMinutes(2);
    date.addSeconds(62);
    date.addMiliseconds(2);
    date.addMonths(1);
    date.addYears(12);

    auto day = date.dayOfWeek();
    EXPECT_EQ(int{date.year()}, 2034);
    EXPECT_EQ(unsigned{date.month()}, 5);
    EXPECT_EQ(unsigned{date.day()}, 1);
    // EXPECT_EQ(day, std::chrono::Thursday);
    EXPECT_EQ(date.hour().count(), 2);
    EXPECT_EQ(date.minute().count(), 14);
    EXPECT_EQ(date.second().count(), 3);
    EXPECT_EQ(date.milisecond().count(), 3);
}

TEST_F(DatioTest, ExampleTest6)
{
    using namespace std::chrono;
    sd::Date date{2022, 3, 10, 20, 11, 1, 1};

    date = date + 21d + 6h + 2min + 62s + 2ms + 12y;
    date.addMonths(1);

    auto day = date.dayOfWeek();
    EXPECT_EQ(int{date.year()}, 2034);
    EXPECT_EQ(unsigned{date.month()}, 5);
    EXPECT_EQ(unsigned{date.day()}, 1);
    // EXPECT_EQ(day, std::chrono::Thursday);
    EXPECT_EQ(date.hour().count(), 2);
    EXPECT_EQ(date.minute().count(), 14);
    EXPECT_EQ(date.second().count(), 3);
    EXPECT_EQ(date.milisecond().count(), 3);
}

TEST_F(DatioTest, ExampleTest7)
{
    using namespace std::chrono;
    sd::Date date{2022, 3, 10, 20, 11, 1, 1};

    date = date - 21d - 6h - 2min - 62s - 2ms - 12y;
    date.substractMonths(1);

    auto day = date.dayOfWeek();
    EXPECT_EQ(int{date.year()}, 2010);
    EXPECT_EQ(unsigned{date.month()}, 1);
    EXPECT_EQ(unsigned{date.day()}, 17);
    // EXPECT_EQ(day, std::chrono::Thursday);
    EXPECT_EQ(date.hour().count(), 14);
    EXPECT_EQ(date.minute().count(), 7);
    EXPECT_EQ(date.second().count(), 58);
    EXPECT_EQ(date.milisecond().count(), 999);
}