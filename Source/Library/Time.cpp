#include "Time.hpp"
#include <chrono>
#include <cmath>
#include <format>
#include <tuple>

namespace sd
{

    using namespace std::chrono;

#define DECOMPOSE(what)                                                                                                \
    auto hh = decomposeHHMMSS(duration).first;                                                                         \
    return hh.what().count() * (hh.is_negative() ? -1 : 1);

    namespace
    {
        template <class D> D roundToZero(Microseconds dur) { return dur < 0 ? ceil<D>(dur) : floor<D>(dur); }

        template <class T, class D> auto cast(D dur) { return duration_cast<T>(dur); }

        template <class D> Time from(int duration) { return Time{cast<Microseconds>(D{duration})}; }
        template <class D> Time from(double duration) { return Time{cast<Microseconds>(D{1} * duration)}; }
        template <class D> double to(Time time) { return time.toDuration<Duration<double, D::period>>().count(); }

        auto decomposeDays(Microseconds duration)
        {
            auto days = roundToZero<Days>(duration);
            auto rest = duration - days;
            return std::make_pair(days, rest);
        }

        auto decomposeHHMMSS(Microseconds duration)
        {
            auto [days, rest] = decomposeDays(duration);
            auto restInMili = roundToZero<Milliseconds>(rest);
            auto restMicroseconds = rest - restInMili;
            HHMMSS hh{restInMili};
            return std::make_pair(hh, restMicroseconds);
        }

        auto decomposeHours(Microseconds duration) { DECOMPOSE(hours); }
        auto decomposeMinutes(Microseconds duration) { DECOMPOSE(minutes); }
        auto decomposeSeconds(Microseconds duration) { DECOMPOSE(seconds); }
        auto decomposeMiliseconds(Microseconds duration) { DECOMPOSE(subseconds); }
        auto decomposeMicroseconds(Microseconds duration) { return decomposeHHMMSS(duration).second.count(); };

        Time parseString(const std::string &source, const std::string &format)
        {
            Microseconds duration;
            std::stringstream ss{source};
            from_stream(ss, format.c_str(), duration);
            return Time{duration};
        }
    } // namespace

    Time Time::max() { return Time{Microseconds::max()}; }
    Time Time::min() { return Time{Microseconds::min()}; }
    Time Time::zero() { return Time{Microseconds::zero()}; }

    Time Time::parse(const std::string &source, const std::string &format) { return parseString(source, format); }

    Time Time::fromDays(int days) { return from<Days>(days); }
    Time Time::fromDays(double days) { return from<Days>(days); }
    Time Time::fromHours(int hours) { return from<Hours>(hours); }
    Time Time::fromHours(double hours) { return from<Hours>(hours); }
    Time Time::fromMinutes(int minutes) { return from<Minutes>(minutes); }
    Time Time::fromMinutes(double minutes) { return from<Minutes>(minutes); }
    Time Time::fromSeconds(int seconds) { return from<Seconds>(seconds); }
    Time Time::fromSeconds(double seconds) { return from<Seconds>(seconds); }
    Time Time::fromMiliseconds(int miliseconds) { return from<Milliseconds>(miliseconds); }
    Time Time::fromMiliseconds(double miliseconds) { return from<Milliseconds>(miliseconds); }
    Time Time::fromMicroseconds(long long microseconds) { return Time{microseconds}; }

    Time::Time(long long microseconds) : _time{Microseconds{microseconds}} {}
    Time::Time(int hour, int minute, int second) : _time{Hours{hour} + Minutes{minute} + Seconds{second}} {}
    Time::Time(int days, int hour, int minute, int second, int miliseconds, int microseconds)
        : _time{Days{days} + Hours{hour} + Minutes{minute} + Seconds{second} + Milliseconds{miliseconds} +
                Microseconds{microseconds}}
    {
    }

    int Time::days() const { return decomposeDays(raw()).first.count(); }
    int Time::hours() const { return decomposeHours(raw()); }
    int Time::minutes() const { return decomposeMinutes(raw()); }
    int Time::seconds() const { return decomposeSeconds(raw()); }
    int Time::miliseconds() const { return decomposeMiliseconds(raw()); }
    int Time::microseconds() const { return decomposeMicroseconds(raw()); }

    double Time::totalDays() const { return to<Days>(raw()); }
    double Time::totalHours() const { return to<Hours>(raw()); }
    double Time::totalMinutes() const { return to<Minutes>(raw()); }
    double Time::totalSeconds() const { return to<Seconds>(raw()); }
    double Time::totalMiliseconds() const { return to<Milliseconds>(raw()); }
    long long Time::totalMicroseconds() const { return raw().count(); }
    Microseconds Time::raw() const { return _time; }

    std::string Time::toString(const std::string &format) const { return std::format(format, raw()); }

#define TIME_OPERATION(operator, operand)                                                                              \
    _time operator operand;                                                                                            \
    return *this;

    Time &Time::add(const Time &other) { TIME_OPERATION(+=, other.raw()); }
    Time &Time::substract(const Time &other) { TIME_OPERATION(-=, other.raw()); }
    Time &Time::multiply(double factor) { TIME_OPERATION(*=, factor); }
    Time &Time::divide(double factor) { TIME_OPERATION(/=, factor); }
    Time &Time::negate() { TIME_OPERATION(=, -_time); }

    Time &Time::operator+=(const Time &other) { return add(other); }
    Time &Time::operator-=(const Time &other) { return substract(other); }
    Time &Time::operator*=(double factor) { return multiply(factor); }
    Time &Time::operator/=(double factor) { return divide(factor); }

    Time Time::operator+() const { return Time{*this}; }
    Time Time::operator-() const { return Time{*this}.negate(); }

    Time operator+(const Time &lhs, const Time &rhs) { return Time{lhs}.add(rhs); }
    Time operator-(const Time &lhs, const Time &rhs) { return Time{lhs}.substract(rhs); }
    Time operator*(const Time &time, double factor) { return Time{time}.multiply(factor); }
    Time operator/(const Time &time, double factor) { return Time{time}.divide(factor); }

    bool operator==(const Time &lhs, const Time &rhs) { return lhs.raw() == rhs.raw(); }
    bool operator!=(const Time &lhs, const Time &rhs) { return lhs.raw() != rhs.raw(); }
    bool operator<(const Time &lhs, const Time &rhs) { return lhs.raw() < rhs.raw(); }
    bool operator<=(const Time &lhs, const Time &rhs) { return lhs.raw() <= rhs.raw(); }
    bool operator>(const Time &lhs, const Time &rhs) { return lhs.raw() > rhs.raw(); }
    bool operator>=(const Time &lhs, const Time &rhs) { return lhs.raw() >= rhs.raw(); }

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