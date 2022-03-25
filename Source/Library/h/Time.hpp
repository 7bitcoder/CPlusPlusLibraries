
#pragma once
#include <chrono>

namespace sd
{

    class Time
    {
      public:
      private:
        std::chrono::microseconds _time;

      public:
        static Time max();
        static Time min();
        static Time zero();

        static Time parse(const std::string &source, const std::string &format = "%T");

        static Time fromDays(int days);
        static Time fromDays(double days);
        static Time fromHours(int hours);
        static Time fromHours(double hours);
        static Time fromMinutes(int minutes);
        static Time fromMinutes(double minutes);
        static Time fromSeconds(int seconds);
        static Time fromSeconds(double seconds);
        static Time fromMiliseconds(int miliseconds);
        static Time fromMiliseconds(double miliseconds);
        static Time fromMicroseconds(long long microseconds);

        template <class Rep, class Period> Time(const std::chrono::duration<Rep, Period> &duration) : _time{duration} {}
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
        std::chrono::microseconds raw() const;

        template <class T> T toChronoDuration() const { return duration_cast<T>(_time); }

        std::string toString(const std::string &format = "{:%T}") const;

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

        Time operator+() const;
        Time operator-() const;

        // operator bool() const;
    };

    Time operator+(const Time &lhs, const Time &rhs);
    Time operator-(const Time &lhs, const Time &rhs);
    Time operator*(const Time &time, double factor);
    Time operator/(const Time &time, double factor);

    bool operator==(const Time &lhs, const Time &rhs);
    bool operator!=(const Time &lhs, const Time &rhs);
    bool operator<(const Time &lhs, const Time &rhs);
    bool operator<=(const Time &lhs, const Time &rhs);
    bool operator>(const Time &lhs, const Time &rhs);
    bool operator>=(const Time &lhs, const Time &rhs);

    Time operator"" _d(long double days);
    Time operator"" _d(unsigned long long days);

    Time operator"" _h(long double hours);
    Time operator"" _h(unsigned long long hours);

    Time operator"" _min(long double minutes);
    Time operator"" _min(unsigned long long minutes);

    Time operator"" _s(long double seconds);
    Time operator"" _s(unsigned long long seconds);

    Time operator"" _ms(long double miliseconds);
    Time operator"" _ms(unsigned long long miliseconds);

    Time operator"" _us(unsigned long long microseconds);
} // namespace sd