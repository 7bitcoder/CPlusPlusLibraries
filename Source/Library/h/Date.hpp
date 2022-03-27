
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
      public:
        static const std::chrono::time_zone *defaultTimeZone;

        static Date parse(const std::string &source, const std::string &format = "%F %T %Z");
        static bool tryParse(Date &date, const std::string &source, const std::string &format = "%F %T %Z");

        static Date max();
        static Date min();
        static Date unixEpoch();

        static Date now();
        static Date nowInUtcTimeZone();
        static Date nowInTimeZone(const std::string &timeZoneName);
        static Date today();

        static int daysInMonth(int year, int month);
        static bool isLeapYear(int year);

        Date(long long microseconds, const std::string &timeZoneName = "");
        Date(int year, int month, int day, const std::string &timeZoneName = "");
        Date(int year, int month, int day, int hour, int minute = 0, int second = 0, int miliseconds = 0,
             const std::string &timeZoneName = "");
        Date(int year, int month, int day, int hour, int minute, int second, int miliseconds,
             const std::chrono::time_zone *timeZone);
        Date(std::chrono::year_month_day date, Time timeOfDay = Time{0}, const std::string &timeZoneName = "");
        Date(std::chrono::year_month_day date, Time timeOfDay, const std::chrono::time_zone *timeZone);

        Date(const Date &) = default;

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

        std::chrono::seconds offset() const;
        const std::chrono::time_zone *timeZone() const;
        std::chrono::time_point<std::chrono::system_clock> timePoint() const;
        std::chrono::time_point<std::chrono::system_clock> zonedTimePoint() const;

        std::string toString(const std::string &format = "{:L%F %T %Z}") const;

        Date toUtcTimeZone() const;
        Date toLocalTimeZone() const;
        Date toDefaultTimeZone() const;
        Date toTimeZone(const std::string &timeZoneName) const;
        Date toTimeZone(const std::chrono::time_zone *timeZone) const;

        Date &changeTimeZoneToUtc();
        Date &changeTimeZoneToLocal();
        Date &changeTimeZoneToDefault();
        Date &changeTimeZone(const std::string &timeZoneName);
        Date &changeTimeZone(const std::chrono::time_zone *timeZone);

        bool isDaylightSavingTime();

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

      private:
        Date(std::chrono::time_point<std::chrono::system_clock> timePoint, const std::string &timeZoneName,
             bool normalize = true);
        Date(std::chrono::time_point<std::chrono::system_clock> timePoint,
             const std::chrono::time_zone *timeZone = nullptr, bool normalize = false);

        template <class Rep, class Period> Date &add(std::chrono::duration<Rep, Period> duration);
        void recomputeOffset();

        std::chrono::time_point<std::chrono::system_clock> _timePoint;
        std::chrono::seconds _offset{0};
        const std::chrono::time_zone *_timeZone{nullptr};
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