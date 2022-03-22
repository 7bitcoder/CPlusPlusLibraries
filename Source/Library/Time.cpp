#include "Time.hpp"
#include <chrono>
#include <format>
#include <tuple>


namespace sd
{

    using namespace std::chrono;
    namespace
    {
        template <class T, class D> Time from(D duration)
        { // 86400000000 microseconds
            Microseconds micro = duration_cast<Microseconds>(T{1});
            auto newValue = static_cast<long long>(duration * micro.count());
            return Time::fromMicroseconds(newValue);
        }

        template <class T> double round(Microseconds dur)
        {
            auto f = duration_cast<duration<double, T::period>>(dur);
            return f.count();
        }

        auto decomposeDays(Microseconds timePoint)
        {
            auto days = floor<Days>(timePoint);
            auto rest = timePoint - days;
            return std::make_pair(days, rest);
        }

        auto decomposeHHMMSS(Microseconds timePoint)
        {
            auto [days, rest] = decomposeDays(timePoint);
            auto restInMili = floor<Milliseconds>(rest);
            auto restMicroseconds = rest - restInMili;
            HHMMSS hh{restInMili};
            return std::make_pair(hh, restMicroseconds);
        }
    } // namespace

    Time Time::max() { return Time{Microseconds::max().count()}; }
    Time Time::min() { return Time{Microseconds::min().count()}; }
    Time Time::zero() { return Time{Microseconds::zero().count()}; }

    Time Time::parse(std::string source, std::string format) {
        Time time{0};
        std::stringstream st{source};
        std::chrono::from_stream(st, format.c_str(), time._time);
        return time;
    }


    Time Time::fromDays(double days) { return from<Days>(days); }
    Time Time::fromHours(double hours) { return from<Hours>(hours); }
    Time Time::fromMinutes(double minutes) { return from<Minutes>(minutes); }
    Time Time::fromSeconds(double seconds) { return from<Seconds>(seconds); }
    Time Time::fromMiliseconds(double miliseconds) { return from<Milliseconds>(miliseconds); }
    Time Time::fromDays(int days) { return from<Days>(days); }
    Time Time::fromHours(int hours) { return from<Hours>(hours); }
    Time Time::fromMinutes(int minutes) { return from<Minutes>(minutes); }
    Time Time::fromSeconds(int seconds) { return from<Seconds>(seconds); }
    Time Time::fromMiliseconds(int miliseconds) { return from<Milliseconds>(miliseconds); }
    Time Time::fromMicroseconds(long long microseconds) { return Time{microseconds}; }

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

    std::string Time::toString(std::string format) const
    {
        return std::format(format, _time);
    }

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

    Time Time::operator+(const Time &other) { return Time{*this}.add(other); }
    Time Time::operator-(const Time &other) { return Time{*this}.substract(other); }
    Time Time::operator*(double factor) { return Time{*this}.multiply(factor); }
    Time Time::operator/(double factor) { return Time{*this}.divide(factor); }

    bool operator==(const Time &lhs, const Time &rhs) { return lhs.totalMicroseconds() == rhs.totalMicroseconds(); }
    bool operator!=(const Time &lhs, const Time &rhs) { return lhs.totalMicroseconds() != rhs.totalMicroseconds(); }
    bool operator<(const Time &lhs, const Time &rhs) { return lhs.totalMicroseconds() < rhs.totalMicroseconds(); }
    bool operator<=(const Time &lhs, const Time &rhs) { return lhs.totalMicroseconds() <= rhs.totalMicroseconds(); }
    bool operator>(const Time &lhs, const Time &rhs) { return lhs.totalMicroseconds() > rhs.totalMicroseconds(); }
    bool operator>=(const Time &lhs, const Time &rhs) { return lhs.totalMicroseconds() >= rhs.totalMicroseconds(); }

    Time Time::operator+() { return Time{*this}; }
    Time Time::operator-() { return Time{*this}.negate(); }

    Time operator""_d(long double days) { return Time::fromDays(static_cast<double>(days)); }
    Time operator""_d(unsigned long long days) { return Time::fromDays(static_cast<int>(days)); }

    Time operator""_h(long double hours) { return Time::fromHours(static_cast<double>(hours)); }
    Time operator""_h(unsigned long long hours) { return Time::fromHours(static_cast<int>(hours)); }

    Time operator""_min(long double minutes) { return Time::fromMinutes(static_cast<double>(minutes)); }
    Time operator""_min(unsigned long long minutes) { return Time::fromMinutes(static_cast<int>(minutes)); }

    Time operator""_s(long double seconds) { return Time::fromSeconds(static_cast<double>(seconds)); }
    Time operator""_s(unsigned long long seconds) { return Time::fromSeconds(static_cast<int>(seconds)); }

    Time operator""_ms(long double miliseconds) { return Time::fromMiliseconds(static_cast<double>(miliseconds)); }
    Time operator""_ms(unsigned long long miliseconds) { return Time::fromMiliseconds(static_cast<int>(miliseconds)); }

    Time operator""_us(unsigned long long microseconds)
    {
        return Time::fromMicroseconds(static_cast<long long>(microseconds));
    }
} // namespace sd