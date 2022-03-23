
#pragma once
#include <chrono>
#include <stdint.h>

namespace sd
{

    using Microseconds = std::chrono::microseconds;

    template <class Rep, class Period = std::ratio<1>> using Duration = std::chrono::duration<Rep, Period>;

    struct Time
    {
      private:
        Microseconds _time;

      public:
        static Time max();
        static Time min();
        static Time zero();

        static Time parse(std::string source, std::string format = "%T");

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

        Time operator+(const Time &other) const;
        Time operator-(const Time &other) const;
        Time operator*(double factor) const;
        Time operator/(double factor) const;

        Time operator+() const;
        Time operator-() const;
    };

    bool operator==(const Time &lhs, const Time &rhs);
    bool operator!=(const Time &lhs, const Time &rhs);
    bool operator<(const Time &lhs, const Time &rhs);
    bool operator<=(const Time &lhs, const Time &rhs);
    bool operator>(const Time &lhs, const Time &rhs);
    bool operator>=(const Time &lhs, const Time &rhs);

    namespace dev
    {
        struct Days : public Time
        {
            Days(int days);
            Days(double days);
            Days(const Days &) = default;
            Days &operator=(const Days &other) = default;
        };
        struct Hours : public Time
        {
            Hours(int hours);
            Hours(double hours);
            Hours(const Hours &) = default;
            Hours &operator=(const Hours &other) = default;
        };
        struct Minutes : public Time
        {
            Minutes(int minutes);
            Minutes(double minutes);
            Minutes(const Minutes &) = default;
            Minutes &operator=(const Minutes &other) = default;
        };
        struct Seconds : public Time
        {
            Seconds(int seconds);
            Seconds(double seconds);
            Seconds(const Seconds &) = default;
            Seconds &operator=(const Seconds &other) = default;
        };
        struct Miliseconds : public Time
        {
            Miliseconds(int miliseconds);
            Miliseconds(double miliseconds);
            Miliseconds(const Miliseconds &) = default;
            Miliseconds &operator=(const Miliseconds &other) = default;
        };
        struct Microseconds : public Time
        {
            Microseconds(long long microseconds);
            Microseconds(const Microseconds &) = default;
            Microseconds &operator=(const Microseconds &other) = default;
        };
    } // namespace dev

    dev::Days operator"" _ds(long double days);
    dev::Days operator"" _ds(unsigned long long days);

    dev::Hours operator"" _h(long double hours);
    dev::Hours operator"" _h(unsigned long long hours);

    dev::Minutes operator"" _min(long double minutes);
    dev::Minutes operator"" _min(unsigned long long minutes);

    dev::Seconds operator"" _s(long double seconds);
    dev::Seconds operator"" _s(unsigned long long seconds);

    dev::Miliseconds operator"" _ms(long double miliseconds);
    dev::Miliseconds operator"" _ms(unsigned long long miliseconds);

    dev::Microseconds operator"" _us(unsigned long long microseconds);
} // namespace sd