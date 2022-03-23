#include "Date.hpp"
#include <chrono>

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
        template <class Rep, class Period> TimePoint createTimePoint(YearMontDay ymd, Duration<Rep, Period> dayTime)
        {
            if (!ymd.ok())
                ymd = ymd.year() / ymd.month() / last;
            return sys_days{ymd} + dayTime;
        }

        TimePointDays toTotalDays(TimePoint timePoint) { return floor<Days>(timePoint); }

        auto decomposeTimePoint(TimePoint timePoint)
        {
            auto days = toTotalDays(timePoint);
            return std::make_pair(YearMontDay{days}, timePoint - days);
        }

        YearMontDay readYearMonthDay(TimePoint timePoint) { return decomposeTimePoint(timePoint).first; }

        auto readTimeOfDay(TimePoint timePoint) { return decomposeTimePoint(timePoint).second; }

        HHMMSS readHMMSS(TimePoint timePoint) { return HHMMSS{floor<Milliseconds>(readTimeOfDay(timePoint))}; }

    } // namespace

    Date Date::now() { return Date{system_clock::now()}; }
    Date Date::max() { return Date{system_clock::now().max()}; }
    Date Date::min() { return Date{system_clock::now().min()}; }

    Date::Date(TimePoint timePoint) { _timePoint = timePoint; }

    Date::Date(long long ticks) { add(Microseconds{ticks}); }

    Date::Date(int year, unsigned month, unsigned day) : Date{year, month, day, 0, 0, 0, 0} {}
    Date::Date(int year, unsigned month, unsigned day, int hour, int minute, int second)
        : Date{year, month, day, hour, minute, second, 0}
    {
    }
    Date::Date(int year, unsigned month, unsigned day, int hour, int minute, int second, int milisecond)
    {
        _timePoint = createTimePoint({Year{year}, Month{month}, Day{day}},
                                     Hours{hour} + Minutes{minute} + Seconds{second} + Milliseconds{milisecond});
    }

    int Date::day() const { return unsigned{readYearMonthDay(_timePoint).day()}; }
    int Date::month() const { return unsigned{readYearMonthDay(_timePoint).month()}; }
    int Date::year() const { return int{readYearMonthDay(_timePoint).year()}; }
    // WeakDay Date::dayOfWeek() const { return {sys_days{getYearMonthDay()}}; }

    int Date::hour() const { return readHMMSS(_timePoint).hours().count(); }
    int Date::minute() const { return readHMMSS(_timePoint).minutes().count(); }
    int Date::second() const { return readHMMSS(_timePoint).seconds().count(); }
    int Date::milisecond() const { return readHMMSS(_timePoint).subseconds().count(); }

    long long Date::ticks() const
    {
        return static_cast<long>(floor<Microseconds>(_timePoint).time_since_epoch().count());
    }

    Date &Date::add(const Time &time) { _timePoint += Microseconds{time.microseconds()}; }

    Date &Date::substract(const Time &time) { return add(-time); }
    Time Date::substract(const Date &time)
    {
        auto diff = _timePoint - time._timePoint;
        return Time{floor<Microseconds>(diff).count()};
    }

    Date &Date::operator+=(const Time &time) { return add(time); }

    Date Date::operator+(const Time &time) { return Date{*this}.add(time); }

    Date &Date::operator-=(const Time &time) { return substract(time); }

    Date Date::operator-(const Time &time) { return Date{*this}.substract(time); }
    Time Date::operator-(const Date &time) { return Date{*this}.substract(time); }

    // template <class Rep, class Period> Date &add22(Duration<Rep, Period> duration)
    // {
    //     if constexpr (Duration<Rep, Period>::period::num < Months::period::num)
    //     {
    //         _timePoint += duration;
    //     }
    //     else
    //     {
    //         auto [ymd, timeOfDay] = decomposeTimePoint(_timePoint);
    //         ymd += duration;
    //         _timePoint = createTimePoint(ymd, timeOfDay);
    //     }
    //     return *this;
    // }

} // namespace sd