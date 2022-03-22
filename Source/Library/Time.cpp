#include "Time.hpp"
#include <chrono>
#include <tuple>

namespace sd
{

    using namespace std::chrono;
    namespace
    {
        template <class T> Time from(double duration)
        {
            Microseconds micro = duration_cast<Microseconds>(T{1});
            micro *= duration;
            return Time::fromMicroseconds(micro);
        }

        template <class T> double round(Microseconds dur)
        {
            auto f = duration_cast<duration<double, T::period>>(dur);
            return f.count();
        }

        auto decomposeDays(Microseconds timePoint)
        {
            auto days = floor<Days>(timePoint);
            auto rest = timePoint = days;
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

    Time Time::fromDays(double days) { return from<Days>(days); }
    Time Time::fromHours(double hours) { return from<Hours>(hours); }
    Time Time::fromMinutes(double minutes) { return from<Minutes>(minutes); }
    Time Time::fromSeconds(double seconds) { return from<Seconds>(seconds); }
    Time Time::fromMiliseconds(double miliseconds) { return from<Milliseconds>(miliseconds); }
    Time Time::fromMicroseconds(Microseconds microseconds) { return Time{microseconds}; }

    Time::Time(Microseconds microseconds) : _time{microseconds} {}
    Time::Time(Hours hour, Minutes minute, Seconds second) : Time{Days{0}, hour, minute, second, 0ms, 0us} {}
    Time::Time(Days days, Hours hour, Minutes minute, Seconds second, Milliseconds miliseconds,
               Microseconds microseconds)
        : _time{days + hour + minute + second + miliseconds + microseconds}
    {
    }

    Time::Time(long long microseconds) : Time{Microseconds{microseconds}} {}
    Time::Time(int hour, int minute, int second) : Time{Hours{hour}, Minutes{minute}, Seconds{second}} {}
    Time::Time(int days, int hour, int minute, int second, int miliseconds, int microseconds)
        : Time{Days{days},
               Hours{hour},
               Minutes{minute},
               Seconds{second},
               Milliseconds{miliseconds},
               Microseconds{microseconds}}
    {
    }

    Days Time::days() const { return decomposeDays(_time).first; }
    Hours Time::hours() const { return decomposeHHMMSS(_time).first.hours(); }
    Minutes Time::minutes() const { return decomposeHHMMSS(_time).first.minutes(); }
    Seconds Time::seconds() const { return decomposeHHMMSS(_time).first.seconds(); }
    Milliseconds Time::miliseconds() const { return decomposeHHMMSS(_time).first.subseconds(); }
    Microseconds Time::microseconds() const { return decomposeHHMMSS(_time).second; }

    double Time::totalDays() const { return round<Days>(_time); }
    double Time::totalHours() const { return round<Hours>(_time); }
    double Time::totalMinutes() const { return round<Minutes>(_time); }
    double Time::totalSeconds() const { return round<Seconds>(_time); }
    double Time::totalMiliseconds() const { return round<Milliseconds>(_time); }
    long long Time::totalMicroseconds() const { return _time.count(); }

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

    Time Time::duration() const { return Time{abs(_time)}; }

    Time &Time::operator+=(const Time &other) { return add(other); }
    Time &Time::operator-=(const Time &other) { return substract(other); }
    Time &Time::operator*=(double factor) { return multiply(factor); }
    Time &Time::operator/=(double factor) { return divide(factor); }

    Time Time::operator+(const Time &other) { return Time{*this}.add(other); }
    Time Time::operator-(const Time &other) { return Time{*this}.substract(other); }
    Time Time::operator*(double factor) { return Time{*this}.multiply(factor); }
    Time Time::operator/(double factor) { return Time{*this}.divide(factor); }
} // namespace sd