
#pragma once
#include "Time.hpp"
#include <chrono>
#include <stdint.h>

namespace sd
{
    using TimePoint = std::chrono::time_point<std::chrono::system_clock>;
    using TimePointDays = std::chrono::time_point<std::chrono::system_clock, Days>;
    class Date
    {
      public:
      private:
        TimePoint _timePoint;

      public:
        static Date now();
        static Date max();
        static Date min();
        static Date today();
        static Date utcNow();

        Date(const Date &) = default;
        Date(long ticks);
        Date(Year year, Month month, Day day);
        Date(Year year, Month month, Day day, Hours hour, Minutes minute, Seconds second);
        Date(Year year, Month month, Day day, Hours hour, Minutes minute, Seconds second, Milliseconds milisecond);

        Date(int year, unsigned month, unsigned day);
        Date(int year, unsigned month, unsigned day, int hour, int minute, int second);
        Date(int year, unsigned month, unsigned day, int hour, int minute, int second, int milisecond);

      private:
        Date(TimePoint timePoint);

      public:
        Year year() const;
        Month month() const;
        Day day() const;
        WeakDay dayOfWeek() const;
        // const int dayOfYear() const;
        Hours hour() const;
        Minutes minute() const;
        Seconds second() const;
        Milliseconds milisecond() const;
        long ticks() const;
        HHMMSS timeOfDay() const;
        YearMontDay getYearMonthDay() const;

        Date &addYears(int years);
        Date &addMonths(int months);
        Date &addDays(int days);
        Date &addHours(int hours);
        Date &addMinutes(int minutes);
        Date &addSeconds(int seconds);
        Date &addMiliseconds(int miliseconds);
        Date &addTicks(long ticks);

        Date &addYears(Years years);
        Date &addMonths(Months months);
        Date &addDays(Days days);
        Date &addYears(Year year);
        Date &addMonths(Month month);
        Date &addDays(Day day);
        Date &addHours(Hours hours);
        Date &addMinutes(Minutes minutes);
        Date &addSeconds(Seconds seconds);
        Date &addMiliseconds(Milliseconds miliseconds);

        Date &substractYears(int years);
        Date &substractMonths(int months);
        Date &substractDays(int days);
        Date &substractHours(int hours);
        Date &substractMinutes(int minutes);
        Date &substractSeconds(int seconds);
        Date &substractMiliseconds(int miliseconds);
        Date &substractTicks(long ticks);

        Date &substractYears(Years years);
        Date &substractMonths(Months months);
        Date &substractDays(Days days);
        Date &substractYears(Year year);
        Date &substractMonths(Month month);
        Date &substractDays(Day day);
        Date &substractHours(Hours hours);
        Date &substractMinutes(Minutes minutes);
        Date &substractSeconds(Seconds seconds);
        Date &substractMiliseconds(Milliseconds miliseconds);

        Date &operator=(const Date &other) = default;

        Date &operator+=(Years years);
        Date &operator+=(Months months);
        Date &operator+=(Days days);
        Date &operator+=(Year year);
        Date &operator+=(Month month);
        Date &operator+=(Day day);
        Date &operator+=(Hours hours);
        Date &operator+=(Minutes minutes);
        Date &operator+=(Seconds seconds);
        Date &operator+=(Milliseconds miliseconds);

        Date operator+(Years years);
        Date operator+(Months months);
        Date operator+(Days days);
        Date operator+(Year year);
        Date operator+(Month month);
        Date operator+(Day day);
        Date operator+(Hours hours);
        Date operator+(Minutes minutes);
        Date operator+(Seconds seconds);
        Date operator+(Milliseconds miliseconds);

        Date &operator-=(Years years);
        Date &operator-=(Months months);
        Date &operator-=(Days days);
        Date &operator-=(Year year);
        Date &operator-=(Month month);
        Date &operator-=(Day day);
        Date &operator-=(Hours hours);
        Date &operator-=(Minutes minutes);
        Date &operator-=(Seconds seconds);
        Date &operator-=(Milliseconds miliseconds);

        Date operator-(Years years);
        Date operator-(Months months);
        Date operator-(Days days);
        Date operator-(Year year);
        Date operator-(Month month);
        Date operator-(Day day);
        Date operator-(Hours hours);
        Date operator-(Minutes minutes);
        Date operator-(Seconds seconds);
        Date operator-(Milliseconds miliseconds);

        static int compare(Date date1, Date date2);
        int compareTo(Date date);
        int daysInMonth(int year, int month);

      private:
        template <class Rep, class Period> Date &add(Duration<Rep, Period> duration);
    };

} // namespace sd