#include <chrono>
#include <gtest/gtest.h>
#include <iostream>
#include <thread>

#include "Date.hpp"

using namespace sd;
using namespace std::chrono;

class DateTest : public ::testing::Test
{
  protected:
    DateTest() {}

    void SetUp() override {}

    void TearDown() override {}

    ~DateTest() {}

    static void TearDownTestSuite() {}
};

#pragma region Constructors

TEST_F(DateTest, MicrosecondsConstructorTest)
{
    Date date{60000000ll};

    EXPECT_EQ(date.year(), 1970);
    EXPECT_EQ(date.month(), 1);
    EXPECT_EQ(date.day(), 1);
    EXPECT_EQ(date.hour(), 0);
    EXPECT_EQ(date.minute(), 1);
    EXPECT_EQ(date.second(), 0);
    EXPECT_EQ(date.milisecond(), 0);
    EXPECT_EQ(date.microsecond(), 0);
}

TEST_F(DateTest, ShortConstructorTest)
{
    Date date{2000, 2, 6};

    EXPECT_EQ(date.year(), 2000);
    EXPECT_EQ(date.month(), 2);
    EXPECT_EQ(date.day(), 6);
    EXPECT_EQ(date.hour(), 0);
    EXPECT_EQ(date.minute(), 0);
    EXPECT_EQ(date.second(), 0);
    EXPECT_EQ(date.milisecond(), 0);
    EXPECT_EQ(date.microsecond(), 0);
}

TEST_F(DateTest, LongConstructorTest)
{
    Date date{2000, 1, 2, 6, 10, 20, 11};

    EXPECT_EQ(date.year(), 2000);
    EXPECT_EQ(date.month(), 1);
    EXPECT_EQ(date.day(), 2);
    EXPECT_EQ(date.hour(), 6);
    EXPECT_EQ(date.minute(), 10);
    EXPECT_EQ(date.second(), 20);
    EXPECT_EQ(date.milisecond(), 11);
    // EXPECT_EQ(date.microsecond(), 123);
}

TEST_F(DateTest, CopyConstructorTest)
{
    Date dateToCopy{2000, 1, 2, 6, 10, 20};
    Date date = dateToCopy;

    EXPECT_EQ(date.year(), 2000);
    EXPECT_EQ(date.month(), 1);
    EXPECT_EQ(date.day(), 2);
    EXPECT_EQ(date.hour(), 6);
    EXPECT_EQ(date.minute(), 10);
    EXPECT_EQ(date.second(), 20);
    EXPECT_EQ(date.milisecond(), 0);
}

TEST_F(DateTest, SpecialConstructorTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms};

    EXPECT_EQ(date.year(), 2011);
    EXPECT_EQ(date.month(), 1);
    EXPECT_EQ(date.day(), 12);
    EXPECT_EQ(date.hour(), 6);
    EXPECT_EQ(date.minute(), 19);
    EXPECT_EQ(date.second(), 11);
    EXPECT_EQ(date.milisecond(), 13);
}

#pragma endregion

#pragma region StaticCreators

#pragma endregion

#pragma region StaticFunctions

TEST_F(DateTest, DaysInMonthSimpleTest) { EXPECT_EQ(Date::daysInMonth(2022, 1), 31); }

TEST_F(DateTest, DaysInMonthFebruaryInLeapYearTest) { EXPECT_EQ(Date::daysInMonth(2024, 2), 29); }

TEST_F(DateTest, DaysInMonthFebruaryInNotLeapYearTest) { EXPECT_EQ(Date::daysInMonth(2022, 2), 28); }

TEST_F(DateTest, InLeapYearTest) { EXPECT_TRUE(Date::isLeapYear(2024)); }

TEST_F(DateTest, InNotLeapYearTest) { EXPECT_FALSE(Date::isLeapYear(2022)); }

#pragma endregion

#pragma region Accessors

TEST_F(DateTest, YearAccessorTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms};

    EXPECT_EQ(date.year(), 2011);
}

TEST_F(DateTest, MonthAccessorTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms};

    EXPECT_EQ(date.month(), 1);
}

TEST_F(DateTest, DayAccessorTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms};

    EXPECT_EQ(date.day(), 12);
}

TEST_F(DateTest, HourAccessorTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms};

    EXPECT_EQ(date.hour(), 6);
}

TEST_F(DateTest, MinuteAccessorTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms};

    EXPECT_EQ(date.minute(), 19);
}

TEST_F(DateTest, SecondAccessorTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms};

    EXPECT_EQ(date.second(), 11);
}

TEST_F(DateTest, MilisecondAccessorTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms};

    EXPECT_EQ(date.milisecond(), 13);
}

TEST_F(DateTest, MicrosecondAccessorTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    EXPECT_EQ(date.microsecond(), 20);
}

TEST_F(DateTest, YearMonthDayAccessorTest)
{
    Date date{2011y / January / 12, 6h + 19min + 11s + 13ms + 20us};

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2011y, January, 12d}));
}

TEST_F(DateTest, TimeOfDayAccessorTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    EXPECT_EQ(date.timeOfDay(), (Time{0, 6, 19, 11, 13, 20}));
}

TEST_F(DateTest, WeekOfMonthAccessorTest)
{
    Date date{2022y / March / 26, 6_h + 19_min + 11_s + 13_ms + 20_us};

    EXPECT_EQ(date.weekOfMonth(), 4);
}

TEST_F(DateTest, WeekOfYearAccessorTest)
{
    Date date{2024y / December / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    // EXPECT_EQ(date.weekOfYear(), 50);
}

TEST_F(DateTest, DayOfWeakAccessorTest)
{
    Date date{2022y / March / 26, 6_h + 19_min + 11_s + 13_ms + 20_us};

    EXPECT_EQ(date.dayOfWeek(), DayOfWeek::Saturday);
}

TEST_F(DateTest, DayOfYearAccessorSimpleTest)
{
    Date date{2022y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    Date yy{2022, 1, 0};

    auto res = date - yy;

    auto hh = res.totalDays();

    EXPECT_EQ(date.dayOfYear(), 12);
}

TEST_F(DateTest, DayOfYearAccessorComplexTest)
{
    Date date{2024y / December / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    EXPECT_EQ(date.dayOfYear(), 347);
}

#pragma endregion

#pragma region AddMethods

TEST_F(DateTest, AddMethodTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    date.add(3_h);

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2011y, January, 12d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 9, 19, 11, 13, 20}));
}

TEST_F(DateTest, AddMethodYearsTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    date.addYears(3);

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2014y, January, 12d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 6, 19, 11, 13, 20}));
}

TEST_F(DateTest, AddMethodMonthsTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    date.addMonths(13);

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2012y, February, 12d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 6, 19, 11, 13, 20}));
}

TEST_F(DateTest, AddMethodDaysTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    date.addDays(30);

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2011y, February, 11d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 6, 19, 11, 13, 20}));
}

TEST_F(DateTest, AddMethodHoursTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    date.addHours(26);

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2011y, January, 13d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 8, 19, 11, 13, 20}));
}

TEST_F(DateTest, AddMethodMinutesTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    date.addMinutes(61);

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2011y, January, 12d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 7, 20, 11, 13, 20}));
}

TEST_F(DateTest, AddMethodSecondsTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    date.addSeconds(61);

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2011y, January, 12d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 6, 20, 12, 13, 20}));
}

TEST_F(DateTest, AddMethodMilisecondsTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    date.addMiliseconds(1001);

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2011y, January, 12d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 6, 19, 12, 14, 20}));
}

TEST_F(DateTest, AddMethodMicrosecondsTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    date.addMicroseconds(1001);

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2011y, January, 12d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 6, 19, 11, 14, 21}));
}

#pragma endregion

#pragma region SubstractMethods

TEST_F(DateTest, SubstractMethodTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    date.substract(3_h);

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2011y, January, 12d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 3, 19, 11, 13, 20}));
}

TEST_F(DateTest, SubstractMethodYearsTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    date.substractYears(3);

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2008y, January, 12d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 6, 19, 11, 13, 20}));
}

TEST_F(DateTest, SubstractMethodMonthsTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    date.substractMonths(13);

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2009y, December, 12d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 6, 19, 11, 13, 20}));
}

TEST_F(DateTest, SubstractMethodDaysTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    date.substractDays(30);

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2010y, December, 13d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 6, 19, 11, 13, 20}));
}

TEST_F(DateTest, SubstractMethodHoursTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    date.substractHours(26);

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2011y, January, 11d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 4, 19, 11, 13, 20}));
}

TEST_F(DateTest, SubstractMethodMinutesTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    date.substractMinutes(61);

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2011y, January, 12d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 5, 18, 11, 13, 20}));
}

TEST_F(DateTest, SubstractMethodSecondsTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    date.substractSeconds(61);

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2011y, January, 12d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 6, 18, 10, 13, 20}));
}

TEST_F(DateTest, SubstractMethodMilisecondsTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    date.substractMiliseconds(1001);

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2011y, January, 12d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 6, 19, 10, 12, 20}));
}

TEST_F(DateTest, SubstractMethodMicrosecondsTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    date.substractMicroseconds(1001);

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2011y, January, 12d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 6, 19, 11, 12, 19}));
}

#pragma endregion

#pragma region AssignOperators

TEST_F(DateTest, BoolOperatorTrueTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    EXPECT_TRUE(date);
}

TEST_F(DateTest, BoolOperatorFalseTest)
{
    Date date = Date{0};

    EXPECT_FALSE(date);
}

TEST_F(DateTest, AssignTest)
{
    Date dateToAssign{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};
    Date date = dateToAssign;

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2011y, January, 12d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 6, 19, 11, 13, 20}));
}

TEST_F(DateTest, AddMethodAssignTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    date += 3_h;

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2011y, January, 12d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 9, 19, 11, 13, 20}));
}

TEST_F(DateTest, AddMethodYearsAssignTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    date += 3_y;

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2014y, January, 12d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 6, 19, 11, 13, 20}));
}

TEST_F(DateTest, AddMethodMonthsAssignTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    date += 13_mo;

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2012y, February, 12d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 6, 19, 11, 13, 20}));
}

TEST_F(DateTest, AddMethodDaysAssignTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    date += 30_d;

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2011y, February, 11d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 6, 19, 11, 13, 20}));
}

TEST_F(DateTest, AddMethodHoursAssignTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    date += 26_h;

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2011y, January, 13d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 8, 19, 11, 13, 20}));
}

TEST_F(DateTest, AddMethodMinutesAssignTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    date += 61_min;

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2011y, January, 12d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 7, 20, 11, 13, 20}));
}

TEST_F(DateTest, AddMethodSecondsAssignTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    date += 61_s;

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2011y, January, 12d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 6, 20, 12, 13, 20}));
}

TEST_F(DateTest, AddMethodMilisecondsAssignTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    date += 1001_ms;

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2011y, January, 12d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 6, 19, 12, 14, 20}));
}

TEST_F(DateTest, AddMethodMicrosecondsAssignTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    date += 1001_us;

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2011y, January, 12d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 6, 19, 11, 14, 21}));
}

TEST_F(DateTest, SubstractMethodAssignTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    date -= 3_h;

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2011y, January, 12d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 3, 19, 11, 13, 20}));
}

TEST_F(DateTest, SubstractMethodYearsAssignTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    date -= 3_y;

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2008y, January, 12d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 6, 19, 11, 13, 20}));
}

TEST_F(DateTest, SubstractMethodMonthsAssignTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    date -= 13_mo;

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2009y, December, 12d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 6, 19, 11, 13, 20}));
}

TEST_F(DateTest, SubstractMethodDaysAssignTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    date -= 30_d;

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2010y, December, 13d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 6, 19, 11, 13, 20}));
}

TEST_F(DateTest, SubstractMethodHoursAssignTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    date -= 26_h;

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2011y, January, 11d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 4, 19, 11, 13, 20}));
}

TEST_F(DateTest, SubstractMethodMinutesAssignTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    date -= 61_min;

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2011y, January, 12d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 5, 18, 11, 13, 20}));
}

TEST_F(DateTest, SubstractMethodSecondsAssignTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    date -= 61_s;

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2011y, January, 12d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 6, 18, 10, 13, 20}));
}

TEST_F(DateTest, SubstractMethodMilisecondsAssignTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    date -= 1001_ms;

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2011y, January, 12d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 6, 19, 10, 12, 20}));
}

TEST_F(DateTest, SubstractMethodMicrosecondsAssignTest)
{
    Date date{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    date -= 1001_us;

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2011y, January, 12d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 6, 19, 11, 12, 19}));
}

#pragma endregion

#pragma region AddTest

TEST_F(DateTest, AddTest)
{
    Date dateToAdd{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    auto date = dateToAdd + 1001_us;

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2011y, January, 12d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 6, 19, 11, 14, 21}));
}

TEST_F(DateTest, AddYearsTest)
{
    Date dateToAdd{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    auto date = dateToAdd + 2_y;

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2013y, January, 12d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 6, 19, 11, 13, 20}));
}

TEST_F(DateTest, AddMonthsTest)
{
    Date dateToAdd{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    auto date = dateToAdd + 13_mo;

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2012y, February, 12d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 6, 19, 11, 13, 20}));
}

#pragma endregion

#pragma region SubstractTest

TEST_F(DateTest, SubstractTest)
{
    Date dateToSubstract{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    auto date = dateToSubstract - 1001_us;

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2011y, January, 12d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 6, 19, 11, 12, 19}));
}

TEST_F(DateTest, SubstractYearsTest)
{
    Date dateToSubstract{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    auto date = dateToSubstract - 2_y;

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2009y, January, 12d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 6, 19, 11, 13, 20}));
}

TEST_F(DateTest, SubstractMonthsTest)
{
    Date dateToSubstract{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    auto date = dateToSubstract - 13_mo;

    EXPECT_EQ(date.yearMonthDay(), (year_month_day{2009y, December, 12d}));
    EXPECT_EQ(date.timeOfDay(), (Time{0, 6, 19, 11, 13, 20}));
}

TEST_F(DateTest, SubstractDateHoursTest)
{
    Date dateToSubstract1{2011y / January / 12, 9_h + 19_min + 11_s + 13_ms + 20_us};

    Date dateToSubstract2{2011y / January / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    auto diff = dateToSubstract1 - dateToSubstract2;

    EXPECT_EQ(diff, (Time{3, 0, 0}));
}

TEST_F(DateTest, SubstractDateDaysTest)
{
    Date dateToSubstract1{2011y / January / 12, 9_h + 19_min + 11_s + 13_ms + 20_us};

    Date dateToSubstract2{2010y / February / 12, 6_h + 19_min + 11_s + 13_ms + 20_us};

    auto diff = dateToSubstract1 - dateToSubstract2;

    EXPECT_EQ(diff, (Time{334, 3, 0, 0, 0, 0}));
}

#pragma endregion

#pragma region CompareTest

TEST_F(DateTest, EqualsTest)
{
    Date date1{2011y / January / 12, 9_h + 19_min + 11_s + 13_ms + 20_us};

    Date date2{2011y / January / 12, Time{0, 9, 19, 11, 13, 20}};

    EXPECT_EQ(date1, date2);
}

TEST_F(DateTest, NotEqualsTest)
{
    Date date1{2011y / January / 12, 9_h + 19_min + 11_s + 13_ms + 20_us};

    Date date2{2010y / February / 12, Time{0, 9, 19, 11, 13, 20}};

    EXPECT_NE(date1, date2);
}

TEST_F(DateTest, GraterTest)
{
    Date date1{2011y / January / 12, 9_h + 19_min + 11_s + 13_ms + 20_us};

    Date date2{2010y / February / 12, Time{0, 9, 19, 11, 13, 20}};

    EXPECT_GT(date1, date2);
}

TEST_F(DateTest, GraterOrEqualTest)
{
    Date date1{2011y / January / 12, 9_h + 19_min + 11_s + 13_ms + 20_us};

    Date date2{2010y / February / 12, Time{0, 9, 19, 11, 13, 20}};

    EXPECT_GE(date1, date2);
}

TEST_F(DateTest, LessTest)
{
    Date date1{2011y / January / 12, 9_h + 19_min + 11_s + 13_ms + 20_us};

    Date date2{2013y / February / 12, Time{0, 9, 19, 11, 13, 20}};

    EXPECT_LT(date1, date2);
}

TEST_F(DateTest, LessOrEqualTest)
{
    Date date1{2011y / January / 12, 9_h + 19_min + 11_s + 13_ms + 20_us};

    Date date2{2013y / February / 12, Time{0, 9, 19, 11, 13, 20}};

    EXPECT_LE(date1, date2);
}

#pragma endregion

#pragma region LiteralsTest

TEST_F(DateTest, YearLiteralTest)
{
    auto year = 12_y;

    EXPECT_EQ(year.value, 12);
}

TEST_F(DateTest, MonthLiteralTest)
{
    auto month = 12_mo;

    EXPECT_EQ(month.value, 12);
}

#pragma endregion

#pragma region ConvertTest

TEST_F(DateTest, ToStringDefaultTest)
{
    Date date{2011y / January / 12, 9_h + 19_min + 11_s + 13_ms + 20_us};

    EXPECT_EQ(date.toString(), std::string{"2011-01-12 09:19:11.0130200 UTC"});
}

TEST_F(DateTest, ToStringVariant1Test)
{
    Date date{2011y / January / 12, 9_h + 19_min + 11_s + 13_ms + 20_us};

    EXPECT_EQ(date.toString("{:%D %T %Z}"), std::string{"01/12/11 09:19:11.0130200 UTC"});
}

TEST_F(DateTest, ToStringMicrosecondsTest)
{
    Date date{2011y / January / 12, 9_h + 19_min + 11_s + 13_ms + 20_us};

    EXPECT_EQ(date.toString("{}"), std::string{"2011-01-12 09:19:11.0130200"});
}

TEST_F(DateTest, ToStringHoursAndMinutesTest)
{
    Date date{2011y / January / 12, 9_h + 19_min + 11_s + 13_ms + 20_us};

    EXPECT_EQ(date.toString("{:%R}"), std::string{"09:19"});
}

TEST_F(DateTest, ToStringNowTest)
{
    Date date = Date::now();

    // EXPECT_EQ(date.toString(), std::string{"09:19"});
}

#pragma endregion

#pragma region Parse

TEST_F(DateTest, ParseDefaultTest)
{
    Date date = Date::parse("2011-01-12 09:19:11.0130200 UTC");

    EXPECT_EQ(date, (Date{2011y / January / 12, 9_h + 19_min + 11_s + 13_ms + 20_us}));
}

TEST_F(DateTest, ParseHoursAndMinutesTest)
{
    Date date = Date::parse("01/12/11 09:19:11.0130200 UTC", "%D %T %Z");

    EXPECT_EQ(date, (Date{2011y / January / 12, 9_h + 19_min + 11_s + 13_ms + 20_us}));
}

TEST_F(DateTest, ParseHoursAndMinutesDefaultTest)
{
    Date date = Date::parse("2011/01/12 09:19:11.0130200 UTC", "");

    EXPECT_EQ(date, (Date{0}));
}

#pragma endregion