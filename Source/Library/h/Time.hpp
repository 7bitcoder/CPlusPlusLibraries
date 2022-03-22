
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
        static Time max();
        static Time min();
        static Time zero();

        static Time fromDays(double days);
        static Time fromHours(double hours);
        static Time fromMinutes(double minutes);
        static Time fromSeconds(double seconds);
        static Time fromMiliseconds(double miliseconds);
        static Time fromDays(int days);
        static Time fromHours(int hours);
        static Time fromMinutes(int minutes);
        static Time fromSeconds(int seconds);
        static Time fromMiliseconds(int miliseconds);
        static Time fromMicroseconds(long long microseconds);

        Time(long long microseconds);
        Time(int hour, int minute, int second);
        Time(int days, int hour, int minute, int second, int miliseconds = 0, int microseconds = 0);

        Time(const Time &other) = default;

        int days() const;
        int hours() const;
        int minutes() const;
        int seconds() const;
        int miliseconds() const;
        int microseconds() const;

        double totalDays() const;
        double totalHours() const;
        double totalMinutes() const;
        double totalSeconds() const;
        double totalMiliseconds() const;
        long long totalMicroseconds() const;

        Time duration() const;

        std::string toString(std::string format = "{:%T}") const;

        Time &add(const Time &other);
        Time &substract(const Time &other);
        Time &multiply(double factor);
        Time &divide(double factor);
        Time &negate();

        Time &operator=(const Time &other) = default;

        Time &operator+=(const Time &other);
        Time &operator-=(const Time &other);
        Time &operator*=(double factor);
        Time &operator/=(double factor);

        Time operator+(const Time &other);
        Time operator-(const Time &other);
        Time operator*(double factor);
        Time operator/(double factor);

        Time operator+();
        Time operator-();
    };

    bool operator==(const Time &lhs, const Time &rhs);
    bool operator!=(const Time &lhs, const Time &rhs);
    bool operator<(const Time &lhs, const Time &rhs);
    bool operator<=(const Time &lhs, const Time &rhs);
    bool operator>(const Time &lhs, const Time &rhs);
    bool operator>=(const Time &lhs, const Time &rhs);

    Time operator""_d(long double days);
    Time operator""_d(unsigned long long days);

    Time operator""_h(long double hours);
    Time operator""_h(unsigned long long hours);

    Time operator""_min(long double minutes);
    Time operator""_min(unsigned long long minutes);

    Time operator""_s(long double seconds);
    Time operator""_s(unsigned long long seconds);

    Time operator""_ms(long double miliseconds);
    Time operator""_ms(unsigned long long miliseconds);

    Time operator""_us(unsigned long long microseconds);
} // namespace sd