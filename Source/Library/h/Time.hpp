
#pragma once
#include <chrono>
#include <stdint.h>

namespace sd
{
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

    template <class Rep, class Period = std::ratio<1>> using Duration = std::chrono::duration<Rep, Period>;

    class Time
    {
      private:
        Microseconds _time;

      public:
        static Time fromDays(double days);
        static Time fromHours(double hours);
        static Time fromMinutes(double minutes);
        static Time fromSeconds(double seconds);
        static Time fromMiliseconds(double miliseconds);
        static Time fromMicroseconds(Microseconds microseconds);

        Time(Microseconds microseconds);
        Time(Hours hour, Minutes minute, Seconds second);
        Time(Days days, Hours hour, Minutes minute, Seconds second, Milliseconds miliseconds = Milliseconds{0},
             Microseconds microseconds = Microseconds{0});

        Time(long long microseconds);
        Time(int hour, int minute, int second);
        Time(int days, int hour, int minute, int second, int miliseconds = 0, int microseconds = 0);

        Time(const Time &other) = default;

        Days days() const;
        Hours hours() const;
        Minutes minutes() const;
        Seconds seconds() const;
        Milliseconds miliseconds() const;
        Microseconds microseconds() const;

        double totalDays() const;
        double totalHours() const;
        double totalMinutes() const;
        double totalSeconds() const;
        double totalMiliseconds() const;
        long long totalMicroseconds() const;

        Time &add(const Time &other);
        Time &substract(const Time &other);
        Time &multiply(double factor);
        Time &divide(double factor);
        Time &negate();

        Time duration() const;

        Time &operator=(const Time &other) = default;
        Time &operator+=(const Time &other);
        Time &operator-=(const Time &other);
        Time &operator*=(double factor);
        Time &operator/=(double factor);

        Time operator+(const Time &other);
        Time operator-(const Time &other);
        Time operator*(double factor);
        Time operator/(double factor);
    };

} // namespace sd