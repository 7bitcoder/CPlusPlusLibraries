
#pragma once
#include <chrono>
#include <stdint.h>

namespace sd
{

    enum DateKind
    {
        Local,
        Unspecified,
        Utc
    };

    class Duration
    {
    };

    class Date
    {
      public:
        using Year = std::chrono::year;
        using Month = std::chrono::month;
        using Day = std::chrono::day;
        using Hours = std::chrono::hours;
        using Minutes = std::chrono::minutes;
        using Seconds = std::chrono::seconds;
        using Milliseconds = std::chrono::milliseconds;

      private:
        std::chrono::time_point<std::chrono::system_clock> _timePoint;

      public:
        static Date now();
        static Date max();
        static Date min();

        Date(uint64_t ticks, DateKind dateKind = DateKind::Local);
        Date(Year year, Month month, Day day, DateKind dateKind = DateKind::Local);
        Date(Year year, Month month, Day day, Hours hour, Minutes minute, Seconds second,
             DateKind dateKind = DateKind::Local);
        Date(Year year, Month month, Day day, Hours hour, Minutes minute, Seconds second, Milliseconds milisecond,
             DateKind dateKind = DateKind::Local);

        Date(int year, unsigned month, unsigned day, DateKind dateKind = DateKind::Local);
        Date(int year, unsigned month, unsigned day, int hour, int minute, int second,
             DateKind dateKind = DateKind::Local);
        Date(int year, unsigned month, unsigned day, int hour, int minute, int second, int milisecond,
             DateKind dateKind = DateKind::Local);

      private:
        Date(std::chrono::time_point<std::chrono::system_clock> timePoint, DateKind dateKind = DateKind::Local);

      public:
        DateKind kind();
        Year year();
        Month month();
        Day day();
        std::chrono::weekday dayOfWeek();
        int dayOfYear();
        Hours hour();
        Minutes minute();
        Seconds second();
        Milliseconds milisecond();
        // int now();
        int ticks();
        int timeOfDay();
        int today();
        int utcNow();

        Date add(Duration duration);
        Date addDays(int days);
        Date addHours(int hours);
        Date addMiliseconds(int miliseconds);
        Date addMinutes(int minutes);
        Date addMonths(int months);
        Date addSeconds(int seconds);
        Date addTicks(int ticks);
        Date addYears(int years);

        static int compare(Date date1, Date date2);
        int compareTo(Date date);
        int daysInMonth(int year, int month);
    };

} // namespace sd