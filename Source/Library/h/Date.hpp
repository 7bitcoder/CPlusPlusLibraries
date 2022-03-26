
#pragma once
#include <chrono>

#include "Time.hpp"

namespace sd
{

    enum DayOfWeek
    {
        Monday = 1,
        Tuesday,
        Wednesday,
        Thursday,
        Friday,
        Saturday,
        Sunday,
    };

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
        static Date utcNow();
        static Date today();
        static Date max();
        static Date min();
        static Date unixEpoch();

        static int daysInMonth(int year, int month);
        static bool isLeapYear(int year);

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

        Time timeOfDay() const;
        int weekOfMonth() const;
        DayOfWeek dayOfWeek() const;
        int dayOfYear() const;

        std::chrono::year_month_day yearMonthDay() const;

        std::chrono::time_point<std::chrono::system_clock> raw() const;

        std::string toString(const std::string &format = "{:%F %T %Z}") const;

        // bool isDaylightSavingTime();

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

        operator bool() const;
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