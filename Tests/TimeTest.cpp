#include <chrono>
#include <gtest/gtest.h>
#include <iostream>
#include <thread>

#include "Date.hpp"

using namespace sd;

class TimeTest : public ::testing::Test
{
  protected:
    TimeTest() {}

    void SetUp() override {}

    void TearDown() override {}

    ~TimeTest() {}

    static void TearDownTestSuite() {}
};

#pragma region Constructors

TEST_F(TimeTest, MicrosecondsConstructorTest)
{
    Time time{60000000ll};

    EXPECT_EQ(time.days(), 0);
    EXPECT_EQ(time.hours(), 0);
    EXPECT_EQ(time.minutes(), 1);
    EXPECT_EQ(time.seconds(), 0);
    EXPECT_EQ(time.miliseconds(), 0);
    EXPECT_EQ(time.microseconds(), 0);
    EXPECT_EQ(time.totalDays(), 1. / 60 / 24);
    EXPECT_EQ(time.totalHours(), 1. / 60);
    EXPECT_EQ(time.totalMinutes(), 1);
    EXPECT_EQ(time.totalSeconds(), 60);
    EXPECT_EQ(time.totalMiliseconds(), 60000);
    EXPECT_EQ(time.totalMicroseconds(), 60000000ll);
}

TEST_F(TimeTest, ShortConstructorTest)
{
    Time time{1, 2, 6};

    EXPECT_EQ(time.days(), 0);
    EXPECT_EQ(time.hours(), 1);
    EXPECT_EQ(time.minutes(), 2);
    EXPECT_EQ(time.seconds(), 6);
    EXPECT_EQ(time.miliseconds(), 0);
    EXPECT_EQ(time.microseconds(), 0);
    EXPECT_EQ(time.totalDays(), 0.043125);
    EXPECT_EQ(time.totalHours(), 1.035);
    EXPECT_EQ(time.totalMinutes(), 62.1);
    EXPECT_EQ(time.totalSeconds(), 3726);
    EXPECT_EQ(time.totalMiliseconds(), 3726000);
    EXPECT_EQ(time.totalMicroseconds(), 3726000000);
}

TEST_F(TimeTest, LongConstructorTest)
{
    Time time{1, 1, 2, 6, 10, 20};

    EXPECT_EQ(time.days(), 1);
    EXPECT_EQ(time.hours(), 1);
    EXPECT_EQ(time.minutes(), 2);
    EXPECT_EQ(time.seconds(), 6);
    EXPECT_EQ(time.miliseconds(), 10);
    EXPECT_EQ(time.microseconds(), 20);
    EXPECT_EQ(time.totalMicroseconds(), 90126010020);
}

TEST_F(TimeTest, CopyConstructorTest)
{
    Time timeToCopy{1, 1, 2, 6, 10, 20};
    Time time = timeToCopy;

    EXPECT_EQ(time.days(), 1);
    EXPECT_EQ(time.hours(), 1);
    EXPECT_EQ(time.minutes(), 2);
    EXPECT_EQ(time.seconds(), 6);
    EXPECT_EQ(time.miliseconds(), 10);
    EXPECT_EQ(time.microseconds(), 20);
    EXPECT_EQ(time.totalMicroseconds(), 90126010020);
}

TEST_F(TimeTest, DurationConstructorTest)
{
    using namespace std::chrono;
    auto duration = 1h + 1min;
    Time time{duration};

    EXPECT_EQ(time.days(), 0);
    EXPECT_EQ(time.hours(), 1);
    EXPECT_EQ(time.minutes(), 1);
    EXPECT_EQ(time.seconds(), 0);
    EXPECT_EQ(time.miliseconds(), 0);
    EXPECT_EQ(time.microseconds(), 0);
    EXPECT_EQ(time.totalMinutes(), 61);
}

#pragma endregion

#pragma region SpecialTypeCreations

TEST_F(TimeTest, FromDaysDoubleTest)
{
    auto time = Time::fromDays(2.0);

    EXPECT_EQ(time, (Time{2, 0, 0, 0, 0, 0}));
    EXPECT_EQ(time.totalDays(), 2.0);
}

TEST_F(TimeTest, FromNegativeDaysDoubleTest)
{
    auto time = Time::fromDays(-2.2);

    EXPECT_EQ(time.totalDays(), -2.2);
    EXPECT_EQ(time.days(), -2);
    EXPECT_EQ(time.hours(), -4);
}

TEST_F(TimeTest, FromDaysIntTest)
{
    auto time = Time::fromDays(2);

    EXPECT_EQ(time, (Time{2, 0, 0, 0, 0, 0}));
    EXPECT_EQ(time.totalDays(), 2.0);
}

TEST_F(TimeTest, FromHoursDoubleTest)
{
    auto time = Time::fromHours(2.0);

    EXPECT_EQ(time, (Time{0, 2, 0, 0, 0, 0}));
    EXPECT_EQ(time.totalHours(), 2.0);
}

TEST_F(TimeTest, FromHoursIntTest)
{
    auto time = Time::fromHours(2);

    EXPECT_EQ(time, (Time{0, 2, 0, 0, 0, 0}));
    EXPECT_EQ(time.totalHours(), 2.0);
}

TEST_F(TimeTest, FromMinutesDoubleTest)
{
    auto time = Time::fromMinutes(2.0);

    EXPECT_EQ(time, (Time{0, 0, 2, 0, 0, 0}));
    EXPECT_EQ(time.totalMinutes(), 2.0);
}

TEST_F(TimeTest, FromMinutesIntTest)
{
    auto time = Time::fromMinutes(2);

    EXPECT_EQ(time, (Time{0, 0, 2, 0, 0, 0}));
    EXPECT_EQ(time.totalMinutes(), 2.0);
}

TEST_F(TimeTest, FromSecondsDoubleTest)
{
    auto time = Time::fromSeconds(2.0);

    EXPECT_EQ(time, (Time{0, 0, 0, 2, 0, 0}));
    EXPECT_EQ(time.totalSeconds(), 2.0);
}

TEST_F(TimeTest, FromSecondsIntTest)
{
    auto time = Time::fromSeconds(2);

    EXPECT_EQ(time, (Time{0, 0, 0, 2, 0, 0}));
    EXPECT_EQ(time.totalSeconds(), 2.0);
}

TEST_F(TimeTest, FromMilisecondsDoubleTest)
{
    auto time = Time::fromMiliseconds(2.0);

    EXPECT_EQ(time, (Time{0, 0, 0, 0, 2, 0}));
    EXPECT_EQ(time.totalMiliseconds(), 2.0);
}

TEST_F(TimeTest, FromMilisecondsIntTest)
{
    auto time = Time::fromMiliseconds(2);

    EXPECT_EQ(time, (Time{0, 0, 0, 0, 2, 0}));
    EXPECT_EQ(time.totalMiliseconds(), 2.0);
}

TEST_F(TimeTest, FromMicrosecondsDoubleTest)
{
    auto time = Time::fromMicroseconds(2.0);

    EXPECT_EQ(time, (Time{0, 0, 0, 0, 0, 2}));
    EXPECT_EQ(time.totalMicroseconds(), 2);
}

TEST_F(TimeTest, FromMicrosecondsIntTest)
    auto time = Time::fromMicroseconds(2);
    EXPECT_EQ(time, (Time{0, 0, 0, 0, 0, 2}));
    EXPECT_EQ(time.totalMicroseconds(), 2);
#pragma endregion

#pragma region StaticConstants

TEST_F(TimeTest, MaxCreatorTest)
{
    auto time = Time::max();

    EXPECT_EQ(time, (Time{std::chrono::microseconds::max().count()}));
}

TEST_F(TimeTest, MinCreatorTest)
{
    auto time = Time::min();

    EXPECT_EQ(time, (Time{std::chrono::microseconds::min().count()}));
}

TEST_F(TimeTest, ZeroCreatorTest)
{
    auto time = Time::zero();

    EXPECT_EQ(time, (Time{0}));
}

#pragma endregion

#pragma region Accessors

TEST_F(TimeTest, DaysTest)
{
    Time time{1, 2, 3, 4, 5, 6};

    EXPECT_EQ(time.days(), 1);
}

TEST_F(TimeTest, HoursTest)
{
    Time time{1, 2, 3, 4, 5, 6};

    EXPECT_EQ(time.hours(), 2);
}

TEST_F(TimeTest, MinutesTest)
{
    Time time{1, 2, 3, 4, 5, 6};

    EXPECT_EQ(time.minutes(), 3);
}

TEST_F(TimeTest, SecondsTest)
{
    Time time{1, 2, 3, 4, 5, 6};

    EXPECT_EQ(time.seconds(), 4);
}

TEST_F(TimeTest, MilisecondsTest)
{
    Time time{1, 2, 3, 4, 5, 6};

    EXPECT_EQ(time.miliseconds(), 5);
}

TEST_F(TimeTest, MicrosecondsTest)
{
    Time time{1, 2, 3, 4, 5, 6};

    EXPECT_EQ(time.microseconds(), 6);
}

TEST_F(TimeTest, NegativeTest)
{
    Time time{-1, -2, -3, -4, -5, -6};

    EXPECT_EQ(time.microseconds(), -6);
}

#pragma endregion

#pragma region TotalAccessorsTest

TEST_F(TimeTest, totalDaysTest)
{
    using namespace std::chrono;
    Time time = 48h;

    EXPECT_EQ(time.totalDays(), 2);
}

TEST_F(TimeTest, totalHoursTest)
{
    using namespace std::chrono;
    Time time = 48h;

    EXPECT_EQ(time.totalHours(), 48);
}

TEST_F(TimeTest, totalMinutesTest)
{
    using namespace std::chrono;
    Time time = 48min;

    EXPECT_EQ(time.totalMinutes(), 48);
}

TEST_F(TimeTest, totalSecondsTest)
{
    using namespace std::chrono;
    Time time = 48s;

    EXPECT_EQ(time.totalSeconds(), 48);
}

TEST_F(TimeTest, totalMilisecondsTest)
{
    using namespace std::chrono;
    Time time = 48ms;

    EXPECT_EQ(time.totalMiliseconds(), 48);
}

TEST_F(TimeTest, totalMicrosecondsTest)
{
    using namespace std::chrono;
    Time time = 48_us;

    EXPECT_EQ(time.totalMicroseconds(), 48);
}

TEST_F(TimeTest, rawTest)
{
    using namespace std::chrono;
    Time time = 48us;

    EXPECT_EQ(time.raw(), Microseconds{48});
}
#pragma endregion

#pragma region UnaryOperators

TEST_F(TimeTest, UnaryAddTest)
{
    auto time = +Time{10000};

    EXPECT_EQ(time.totalMicroseconds(), 10000);
}

TEST_F(TimeTest, UnaryNegativeTest)
{
    auto time = -Time{10000};

    EXPECT_EQ(time.totalMicroseconds(), -10000);
}

#pragma endregion

#pragma region MethodOperations

TEST_F(TimeTest, MethodAddTest)
{
    Time time{10000};

    time.add(Time{10000});

    EXPECT_EQ(time.totalMicroseconds(), 20000);
}

TEST_F(TimeTest, MethodSubsrtactTest)
{
    Time time{10000};

    time.substract(Time{50000});

    EXPECT_EQ(time.totalMicroseconds(), -40000);
}

TEST_F(TimeTest, MethodMultiplyTest)
{
    Time time{10000};

    time.multiply(2);

    EXPECT_EQ(time.totalMicroseconds(), 20000);
}

TEST_F(TimeTest, MethodDivideTest)
{
    Time time{10000};

    time.divide(2);

    EXPECT_EQ(time.totalMicroseconds(), 5000);
}

TEST_F(TimeTest, MethodNegateTest)
{
    Time time{10000};

    time.negate();

    EXPECT_EQ(time.totalMicroseconds(), -10000);
}

#pragma endregion

#pragma region AssignmentsOperators

TEST_F(TimeTest, AssignementTest)
{
    Time timeToAssign{10000};
    Time time{900};
    time = timeToAssign;

    EXPECT_EQ(time.totalMicroseconds(), 10000);
}

TEST_F(TimeTest, AddAssignmentTest)
{
    Time timeToAssign{10000};
    Time time{100};
    time += timeToAssign;

    EXPECT_EQ(time.totalMicroseconds(), 10100);
}

TEST_F(TimeTest, SubstractAssignmentTest)
{
    Time timeToAssign{10000};
    Time time{100};
    time -= timeToAssign;

    EXPECT_EQ(time.totalMicroseconds(), -9900);
}

TEST_F(TimeTest, MultiplyAssignmentTest)
{
    Time time{100};
    time *= 2;

    EXPECT_EQ(time.totalMicroseconds(), 200);
}

TEST_F(TimeTest, DivideAssignmentTest)
{
    Time time{100};
    time /= 2;

    EXPECT_EQ(time.totalMicroseconds(), 50);
}

#pragma endregion

#pragma region OperatorsTest

TEST_F(TimeTest, AddOperatorTest)
{
    Time time1{10000};
    Time time2{100};
    Time time = time1 + time2;

    EXPECT_EQ(time.totalMicroseconds(), 10100);
}

TEST_F(TimeTest, SubstractOperatorTest)
{
    Time time1{100};
    Time time2{10000};
    Time time = time1 - time2;

    EXPECT_EQ(time.totalMicroseconds(), -9900);
}

TEST_F(TimeTest, MultiplyOperatorTest)
{
    Time time1{100};
    Time time = time1 * 2;

    EXPECT_EQ(time.totalMicroseconds(), 200);
}

TEST_F(TimeTest, DivideOperatorTest)
{
    Time time1{100};
    Time time = time1 / 2;

    EXPECT_EQ(time.totalMicroseconds(), 50);
}

#pragma endregion

#pragma region LiteralOperators

TEST_F(TimeTest, DayLiteralTest)
{
    Time time = 2_d;

    EXPECT_EQ(time.totalDays(), 2);
}

TEST_F(TimeTest, DayLiteralDoubleTest)
{
    Time time = 2.2_d;

    EXPECT_EQ(time.totalDays(), 2.2);
}

TEST_F(TimeTest, HourLiteralTest)
{
    Time time = 2_h;

    EXPECT_EQ(time.totalHours(), 2);
}

TEST_F(TimeTest, HourLiteralDoubleTest)
{
    Time time = 2.2_h;

    EXPECT_EQ(time.totalHours(), 2.2);
}

TEST_F(TimeTest, MinuteLiteralTest)
{
    Time time = 2_min;

    EXPECT_EQ(time.totalMinutes(), 2);
}

TEST_F(TimeTest, MinuteLiteralDoubleTest)
{
    Time time = 2.2_min;

    EXPECT_EQ(time.totalMinutes(), 2.2);
}

TEST_F(TimeTest, SecondLiteralTest)
{
    Time time = 2_s;

    EXPECT_EQ(time.totalSeconds(), 2);
}

TEST_F(TimeTest, SecondLiteralDoubleTest)
{
    Time time = 2.2_s;

    EXPECT_EQ(time.totalSeconds(), 2.2);
}

TEST_F(TimeTest, MilisecondLiteralTest)
{
    Time time = 2_ms;

    EXPECT_EQ(time.totalMiliseconds(), 2);
}

TEST_F(TimeTest, MilisecondLiteralDoubleTest)
{
    Time time = 2.2_ms;

    EXPECT_EQ(time.totalMiliseconds(), 2.2);
}

TEST_F(TimeTest, MicrosecondLiteralTest)
{
    Time time = 2_us;

    EXPECT_EQ(time.totalMicroseconds(), 2);
}

#pragma endregion

#pragma region ComparisonTest

TEST_F(TimeTest, EqualTest)
{
    Time time1 = 2_h;
    Time time2 = Time::fromMinutes(120);

    EXPECT_EQ(time1, time2);
}

TEST_F(TimeTest, NotEqualTest)
{
    Time time1 = 3_h;
    Time time2 = Time::fromMinutes(120);

    EXPECT_NE(time1, time2);
}

TEST_F(TimeTest, GraterTest)
{
    Time time1 = 3_h;
    Time time2 = Time::fromMinutes(120);

    EXPECT_GT(time1, time2);
}

TEST_F(TimeTest, GraterOrEqualTest)
{
    Time time1 = 3_h;
    Time time2 = Time::fromMinutes(120);

    EXPECT_GE(time1, time2);
}

TEST_F(TimeTest, LessTest)
{
    Time time1 = 1_h;
    Time time2 = Time::fromMinutes(120);

    EXPECT_LT(time1, time2);
}

TEST_F(TimeTest, LessOrEqualTest)
{
    Time time1 = 1_h;
    Time time2 = Time::fromMinutes(120);

    EXPECT_LE(time1, time2);
}

#pragma endregion

#pragma region ConvertTest

TEST_F(TimeTest, ToStringDefaultTest)
{
    Time time = 2_d + 2_h + 1_min + 1_s + 1_ms + 2_us;

    EXPECT_EQ(time.toString(), std::string{"02:01:01.001002"});
}

TEST_F(TimeTest, ToStringMicrosecondsTest)
{
    Time time = 2_d + 2_h + 1_min + 1_s + 1_ms + 2_us;

    EXPECT_EQ(time.toString("{}"), std::string{"180061001002us"});
}

TEST_F(TimeTest, ToStringHoursAndMinutesTest)
{
    Time time = 2_d + 2_h + 1_min + 1_s + 1_ms + 2_us;

    EXPECT_EQ(time.toString("{:%R}"), std::string{"02:01"});
}

TEST_F(TimeTest, ToDurationTest)
{
    Time time = 2_d + 2_h + 1_min + 1_s + 1_ms + 2_us;

    EXPECT_EQ(time.toDuration<Minutes>(), (Minutes{3001}));
}
#pragma endregion

#pragma region Parse

TEST_F(TimeTest, ParseDefaultTest)
{
    Time time = Time::parse("02:01:01.001002");

    EXPECT_EQ(time, (Time{0, 2, 1, 1, 1, 2}));
}

TEST_F(TimeTest, ParseHoursAndMinutesTest)
{
    Time time = Time::parse("02:01", "%R");

    EXPECT_EQ(time, (Time{0, 2, 1, 0}));
}

#pragma endregion