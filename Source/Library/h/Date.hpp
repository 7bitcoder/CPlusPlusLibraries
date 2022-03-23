
#pragma once
#include "Time.hpp"
#include <chrono>
#include <stdint.h>

namespace sd
{
    using TimePoint = std::chrono::time_point<std::chrono::system_clock>;
    using TimePointDays = std::chrono::time_point<std::chrono::system_clock, std::chrono::days>;
    struct Date
    {
      private:
        TimePoint _timePoint;

      public:
        static Date now();
        static Date max();
        static Date min();
        static Date today();
        static Date utcNow();

        Date(long long ticks);
        Date(int year, unsigned month, unsigned day);
        Date(int year, unsigned month, unsigned day, int hour, int minute, int second);
        Date(int year, unsigned month, unsigned day, int hour, int minute, int second, int milisecond);

        Date(const Date &) = default;

      private:
        Date(TimePoint timePoint);

      public:
        int year() const;
        int month() const;
        int day() const;
        // int dayOfWeek() const;
        // const int dayOfYear() const;
        int hour() const;
        int minute() const;
        int second() const;
        int milisecond() const;
        long long ticks() const;

        Date &add(const Time &time);

        Time substract(const Date &time);
        Date &substract(const Time &time);

        Date &operator=(const Date &other) = default;

        Date &operator+=(const Time &time);

        Date operator+(const Time &time);

        Date &operator-=(const Time &time);

        Time operator-(const Date &time);
        Date operator-(const Time &time);

      private:
        // template <class Rep, class Period> Date &add(Duration<Rep, Period> duration);
    };
} // namespace sd