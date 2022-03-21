
#pragma once
#include <chrono>
#include <stdint.h>

namespace sd
{

    class Date
    {
      public:
        using Year = std::chrono::year;
        using Month = std::chrono::month;
        using Day = std::chrono::day;
        using Years = std::chrono::years;
        using Months = std::chrono::months;
        using Days = std::chrono::days;
        using Weaks = std::chrono::weeks;
        using WeakDay = std::chrono::weekday;
        using Hours = std::chrono::hours;
        using Minutes = std::chrono::minutes;
        using Seconds = std::chrono::seconds;
        using Milliseconds = std::chrono::milliseconds;
        using Microseconds = std::chrono::microseconds;
        using YearMontDay = std::chrono::year_month_day;
        using HHMMSS = std::chrono::hh_mm_ss<Milliseconds>;

        using TimePoint = std::chrono::time_point<std::chrono::system_clock>;
        using TimePointDays = std::chrono::time_point<std::chrono::system_clock, Days>;

        template <class Rep, class Period = std::ratio<1>> using Duration = std::chrono::duration<Rep, Period>;

      private:
        TimePoint _timePoint;

      public:
        static Date now();
        static Date max();
        static Date min();

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
        const int dayOfYear() const;
        Hours hour() const;
        Minutes minute() const;
        Seconds second() const;
        Milliseconds milisecond() const;
        // int now();
        long ticks() const;
        int timeOfDay();
        const int today() const;
        int utcNow() const;

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

        YearMontDay getYearMonthDay() const;
        HHMMSS getHHMMSS() const;

      private:
        TimePointDays getTotalDays() const;
        template <class Rep, class Period = std::ratio<1>> Date &add(Duration<Rep, Period> duration);
        template <class Rep, class Period = std::ratio<1>> Date &addDate(Duration<Rep, Period> duration);
        template <class Rep, class Period = std::ratio<1>> Date &substract(Duration<Rep, Period> duration);
        template <class Rep, class Period = std::ratio<1>> Date &substractDate(Duration<Rep, Period> duration);
    };

} // namespace sd