#include "Time.hpp"
#include <chrono>
#include <format>
#include <tuple>

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
    using YearMontDay = std::chrono::year_month_day;
    using HHMMSS = std::chrono::hh_mm_ss<Milliseconds>;

    using namespace std::chrono;
    namespace
    {
        template <class T> T roundToZero(Microseconds timePoint)
        {
            bool isLessThanZero = timePoint < Microseconds::zero();
            return isLessThanZero ? ceil<T>(timePoint) : floor<T>(timePoint);
        }

        template <class T, class D> long long from(D duration)
        {
            Microseconds micro = duration_cast<Microseconds>(T{1});
            return static_cast<long long>(duration * micro.count());
        }

        template <class T> double round(Microseconds dur)
        {
            auto f = duration_cast<duration<double, T::period>>(dur);
            return f.count();
        }

        auto decomposeDays(Microseconds timePoint)
        {
            auto days = roundToZero<Days>(timePoint);
            auto rest = timePoint - days;
            return std::make_pair(days, rest);
        }

        auto decomposeHHMMSS(Microseconds timePoint)
        {
            auto [days, rest] = decomposeDays(timePoint);
            auto restInMili = roundToZero<Milliseconds>(rest);
            auto restMicroseconds = rest - restInMili;
            HHMMSS hh{restInMili};
            return std::make_pair(hh, restMicroseconds);
        }
    } // namespace

    Time Time::max() { return Time{Microseconds::max().count()}; }
    Time Time::min() { return Time{Microseconds::min().count()}; }
    Time Time::zero() { return Time{Microseconds::zero().count()}; }

    Time Time::parse(std::string source, std::string format)
    {
        Time time{0};
        std::stringstream st{source};
        std::chrono::from_stream(st, format.c_str(), time._time);
        return time;
    }

    namespace dev
    {
        namespace ch = std::chrono;

        Days::Days(int days) : Time{from<ch::days>(days)} {}
        Days::Days(double days) : Time{from<ch::days>(days)} {}

        Hours::Hours(int hours) : Time{from<ch::hours>(hours)} {}
        Hours::Hours(double hours) : Time{from<ch::hours>(hours)} {}

        Minutes::Minutes(int minutes) : Time{from<ch::minutes>(minutes)} {}
        Minutes::Minutes(double minutes) : Time{from<ch::minutes>(minutes)} {}

        Seconds::Seconds(int seconds) : Time{from<ch::seconds>(seconds)} {}
        Seconds::Seconds(double seconds) : Time{from<ch::seconds>(seconds)} {}

        Miliseconds::Miliseconds(int miliseconds) : Time{from<ch::milliseconds>(miliseconds)} {}
        Miliseconds::Miliseconds(double miliseconds) : Time{from<ch::milliseconds>(miliseconds)} {}

        Microseconds::Microseconds(long long microseconds) : Time{microseconds} {}
    } // namespace dev

    Time::Time(long long microseconds) : _time{Microseconds{microseconds}} {}
    Time::Time(int hour, int minute, int second) : _time{Hours{hour} + Minutes{minute} + Seconds{second}} {}
    Time::Time(int days, int hour, int minute, int second, int miliseconds, int microseconds)
        : _time{Days{days} + Hours{hour} + Minutes{minute} + Seconds{second} + Milliseconds{miliseconds} +
                Microseconds{microseconds}}
    {
    }

    int Time::days() const { return decomposeDays(_time).first.count(); }
    int Time::hours() const { return decomposeHHMMSS(_time).first.hours().count(); }
    int Time::minutes() const { return decomposeHHMMSS(_time).first.minutes().count(); }
    int Time::seconds() const { return decomposeHHMMSS(_time).first.seconds().count(); }
    int Time::miliseconds() const { return decomposeHHMMSS(_time).first.subseconds().count(); }
    int Time::microseconds() const { return decomposeHHMMSS(_time).second.count(); }

    double Time::totalDays() const { return round<Days>(_time); }
    double Time::totalHours() const { return round<Hours>(_time); }
    double Time::totalMinutes() const { return round<Minutes>(_time); }
    double Time::totalSeconds() const { return round<Seconds>(_time); }
    double Time::totalMiliseconds() const { return round<Milliseconds>(_time); }
    long long Time::totalMicroseconds() const { return _time.count(); }

    Time Time::duration() const { return Time{abs(_time).count()}; }

    std::string Time::toString(std::string format) const { return std::format(format, _time); }

    Time &Time::add(const Time &other)
    {
        _time += other._time;
        return *this;
    }
    Time &Time::substract(const Time &other)
    {
        _time -= other._time;
        return *this;
    }
    Time &Time::multiply(double factor)
    {
        _time *= factor;
        return *this;
    }
    Time &Time::divide(double factor)
    {
        _time /= factor;
        return *this;
    }
    Time &Time::negate()
    {
        _time = -_time;
        return *this;
    }

    Time &Time::operator+=(const Time &other) { return add(other); }
    Time &Time::operator-=(const Time &other) { return substract(other); }
    Time &Time::operator*=(double factor) { return multiply(factor); }
    Time &Time::operator/=(double factor) { return divide(factor); }

    Time Time::operator+(const Time &other) const { return Time{*this}.add(other); }
    Time Time::operator-(const Time &other) const { return Time{*this}.substract(other); }
    Time Time::operator*(double factor) const { return Time{*this}.multiply(factor); }
    Time Time::operator/(double factor) const { return Time{*this}.divide(factor); }

    bool operator==(const Time &lhs, const Time &rhs) { return lhs.totalMicroseconds() == rhs.totalMicroseconds(); }
    bool operator!=(const Time &lhs, const Time &rhs) { return lhs.totalMicroseconds() != rhs.totalMicroseconds(); }
    bool operator<(const Time &lhs, const Time &rhs) { return lhs.totalMicroseconds() < rhs.totalMicroseconds(); }
    bool operator<=(const Time &lhs, const Time &rhs) { return lhs.totalMicroseconds() <= rhs.totalMicroseconds(); }
    bool operator>(const Time &lhs, const Time &rhs) { return lhs.totalMicroseconds() > rhs.totalMicroseconds(); }
    bool operator>=(const Time &lhs, const Time &rhs) { return lhs.totalMicroseconds() >= rhs.totalMicroseconds(); }

    Time Time::operator+() const { return Time{*this}; }
    Time Time::operator-() const { return Time{*this}.negate(); }

    dev::Days operator""_ds(long double days) { return dev::Days(static_cast<double>(days)); }
    dev::Days operator""_ds(unsigned long long days) { return dev::Days(static_cast<int>(days)); }

    dev::Hours operator""_h(long double hours) { return dev::Hours(static_cast<double>(hours)); }
    dev::Hours operator""_h(unsigned long long hours) { return dev::Hours(static_cast<int>(hours)); }

    dev::Minutes operator""_min(long double minutes) { return dev::Minutes(static_cast<double>(minutes)); }
    dev::Minutes operator""_min(unsigned long long minutes) { return dev::Minutes(static_cast<int>(minutes)); }

    dev::Seconds operator""_s(long double seconds) { return dev::Seconds(static_cast<double>(seconds)); }
    dev::Seconds operator""_s(unsigned long long seconds) { return dev::Seconds(static_cast<int>(seconds)); }

    dev::Miliseconds operator""_ms(long double miliseconds)
    {
        return dev::Miliseconds(static_cast<double>(miliseconds));
    }
    dev::Miliseconds operator""_ms(unsigned long long miliseconds)
    {
        return dev::Miliseconds(static_cast<int>(miliseconds));
    }

    dev::Microseconds operator""_us(unsigned long long microseconds)
    {
        return dev::Microseconds(static_cast<long long>(microseconds));
    }
} // namespace sd