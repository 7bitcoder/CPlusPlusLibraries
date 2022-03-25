
#pragma once
#include <chrono>

#include "Time.hpp"

namespace sd
{

    struct Years
    {
        int value;
        Years(int value) : value(value){};
    };

    struct Months
    {
        int value;
        Months(int value) : value(value){};
    };

    struct Date
    {
      private:
        std::chrono::time_point<std::chrono::system_clock> _timePoint;

      public:
        static Date parse(const std::string &source, const std::string &format = "%F %T %Z");
        static Date now();
        static Date max();
        static Date min();
        static Date today();

        Date(long long microseconds);
        Date(int year, unsigned month, unsigned day, int hour = 0, int minute = 0, int second = 0, int miliseconds = 0);
        Date(std::chrono::year_month_day date, Time timeOfDay = Time{0});

        Date(const Date &) = default;

      private:
        Date(std::chrono::time_point<std::chrono::system_clock> timePoint);

      public:
        int year() const;
        int month() const;
        int day() const;
        int hour() const;
        int minute() const;
        int second() const;
        int milisecond() const;
        long long microsecond() const;

        std::chrono::time_point<std::chrono::system_clock> raw() const;

        std::chrono::year_month_day yearMonthDay() const;
        Time timeOfDay() const;

        std::string toString(const std::string &format = "{:%F %T %Z}") const;

        Date &add(const Time &time);
        Date &addYears(int years);
        Date &addMonths(int months);
        Date &addDays(int days);
        Date &addHours(int hours);
        Date &addMinutes(int minutes);
        Date &addSeconds(int seconds);
        Date &addMiliseconds(int miliseconds);
        Date &addMicroseconds(long long microseconds);

        Time substract(const Date &date) const;
        Date &substract(const Time &time);
        Date &substractYears(int years);
        Date &substractMonths(int months);
        Date &substractDays(int days);
        Date &substractHours(int hours);
        Date &substractMinutes(int minutes);
        Date &substractSeconds(int seconds);
        Date &substractMiliseconds(int miliseconds);
        Date &substractMicroseconds(long long microseconds);

        Date &operator=(const Date &other) = default;

        Date &operator+=(const Time &time);
        Date &operator+=(const Years &years);
        Date &operator+=(const Months &months);

        Date &operator-=(const Time &time);
        Date &operator-=(const Years &years);
        Date &operator-=(const Months &months);
    };

    Date operator+(const Date &date, const Time &time);
    Date operator+(const Date &date, const Years &years);
    Date operator+(const Date &date, const Months &months);

    Date operator-(const Date &date, const Time &time);
    Date operator-(const Date &date, const Years &years);
    Date operator-(const Date &date, const Months &months);

    Time operator-(const Date &lhs, const Date &rhs);

    bool operator==(const Date &lhs, const Date &rhs);
    bool operator!=(const Date &lhs, const Date &rhs);
    bool operator<(const Date &lhs, const Date &rhs);
    bool operator<=(const Date &lhs, const Date &rhs);
    bool operator>(const Date &lhs, const Date &rhs);
    bool operator>=(const Date &lhs, const Date &rhs);

    Years operator"" _y(unsigned long long years);
    Months operator"" _mo(unsigned long long months);

} // namespace sd