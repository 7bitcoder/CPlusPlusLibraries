#include "Datio.hpp"
#include <chrono>
#include <time.h>

using namespace std::chrono;
namespace sd
{
    namespace
    {
    }

    Date Date::now() { return Date{system_clock::now()}; }
    Date Date::max() { return Date{system_clock::now().max()}; }
    Date Date::min() { return Date{system_clock::now().min()}; }

    Date::Date(TimePoint timePoint) { _timePoint = timePoint; }

    Date::Date(long ticks) { addTicks(ticks); }

    Date::Date(int year, unsigned month, unsigned day) : Date{Year{year}, Month{month}, Day{day}} {}
    Date::Date(int year, unsigned month, unsigned day, int hour, int minute, int second)
        : Date{Year{year}, Month{month}, Day{day}, Hours{hour}, Minutes{minute}, Seconds{second}}
    {
    }
    Date::Date(int year, unsigned month, unsigned day, int hour, int minute, int second, int milisecond)
        : Date{Year{year},
               Month{month},
               Day{day},
               Hours{hour},
               Minutes{minute},
               Seconds{second},
               Milliseconds{milisecond}}
    {
    }

    Date::Date(Year year, Month month, Day day) : Date{year, month, day, 0h, 0min, 0s, 0ms} {}
    Date::Date(Year year, Month month, Day day, Hours hour, Minutes minute, Seconds second)
        : Date{year, month, day, hour, minute, second, 0ms}
    {
    }
    Date::Date(Year year, Month month, Day day, Hours hour, Minutes minute, Seconds second, Milliseconds milisecond)
    {
        YearMontDay date{year, month, day};

        TimePoint ld = sys_days{date} + hour + minute + second + milisecond;

        _timePoint = ld;
    }

    Date::Day Date::day() const { return getYearMonthDay().day(); }
    Date::Month Date::month() const { return getYearMonthDay().month(); }
    Date::Year Date::year() const { return getYearMonthDay().year(); }
    Date::WeakDay Date::dayOfWeek() const { return {sys_days{getYearMonthDay()}}; }

    Date::Hours Date::hour() const { return getHHMMSS().hours(); }
    Date::Minutes Date::minute() const { return getHHMMSS().minutes(); }
    Date::Seconds Date::second() const { return getHHMMSS().seconds(); }
    Date::Milliseconds Date::milisecond() const { return getHHMMSS().subseconds(); }

    long Date::ticks() const
    {
        return static_cast<long>(floor<Date::Microseconds>(_timePoint).time_since_epoch().count());
    }

    Date::YearMontDay Date::getYearMonthDay() const { return {getTotalDays()}; }

    Date::TimePointDays Date::getTotalDays() const { return floor<Date::Days>(_timePoint); }

    Date::HHMMSS Date::getHHMMSS() const { return HHMMSS{floor<Milliseconds>(_timePoint - getTotalDays())}; }

    Date &Date::addYears(int years) { return addYears(Years{years}); }
    Date &Date::addMonths(int months) { return addMonths(Months{months}); }
    Date &Date::addDays(int days) { return addDays(Days{days}); }
    Date &Date::addHours(int hours) { return addHours(Hours{hours}); }
    Date &Date::addMinutes(int minutes) { return addMinutes(Minutes{minutes}); }
    Date &Date::addSeconds(int seconds) { return addSeconds(Seconds{seconds}); }
    Date &Date::addMiliseconds(int miliseconds) { return addMiliseconds(Milliseconds{miliseconds}); }
    Date &Date::addTicks(long ticks) { return add(Microseconds{ticks}); }

    Date &Date::addYears(Years years) { return addDate(years); }
    Date &Date::addMonths(Months months) { return addDate(months); }
    Date &Date::addDays(Days days) { return add(days); }
    Date &Date::addYears(Year year) { return addYears(Years{int(year)}); }
    Date &Date::addMonths(Month month) { return addMonths(Months{unsigned(month)}); }
    Date &Date::addDays(Day day) { return addDays(Days{unsigned(day)}); }
    Date &Date::addHours(Hours hours) { return add(hours); }
    Date &Date::addMinutes(Minutes minutes) { return add(minutes); }
    Date &Date::addSeconds(Seconds seconds) { return add(seconds); }
    Date &Date::addMiliseconds(Milliseconds miliseconds) { return add(miliseconds); }

    Date &Date::operator+=(Years years) { return addYears(years); }
    Date &Date::operator+=(Months months) { return addMonths(months); }
    Date &Date::operator+=(Days days) { return addDays(days); }
    Date &Date::operator+=(Year year) { return addYears(year); }
    Date &Date::operator+=(Month month) { return addMonths(month); }
    Date &Date::operator+=(Day day) { return addDays(day); }
    Date &Date::operator+=(Hours hours) { return addHours(hours); }
    Date &Date::operator+=(Minutes minutes) { return addMinutes(minutes); }
    Date &Date::operator+=(Seconds seconds) { return addSeconds(seconds); }
    Date &Date::operator+=(Milliseconds miliseconds) { return addMiliseconds(miliseconds); }

    Date Date::operator+(Years years) { return Date{*this}.addYears(years); }
    Date Date::operator+(Months months) { return Date{*this}.addMonths(months); }
    Date Date::operator+(Days days) { return Date{*this}.addDays(days); }
    Date Date::operator+(Year year) { return Date{*this}.addYears(year); }
    Date Date::operator+(Month month) { return Date{*this}.addMonths(month); }
    Date Date::operator+(Day day) { return Date{*this}.addDays(day); }
    Date Date::operator+(Hours hours) { return Date{*this}.addHours(hours); }
    Date Date::operator+(Minutes minutes) { return Date{*this}.addMinutes(minutes); }
    Date Date::operator+(Seconds seconds) { return Date{*this}.addSeconds(seconds); }
    Date Date::operator+(Milliseconds miliseconds) { return Date{*this}.addMiliseconds(miliseconds); }

    Date &Date::substractYears(int years) { return substractYears(Years{years}); }
    Date &Date::substractMonths(int months) { return substractMonths(Months{months}); }
    Date &Date::substractDays(int days) { return substractDays(Days{days}); }
    Date &Date::substractHours(int hours) { return substractHours(Hours{hours}); }
    Date &Date::substractMinutes(int minutes) { return substractMinutes(Minutes{minutes}); }
    Date &Date::substractSeconds(int seconds) { return substractSeconds(Seconds{seconds}); }
    Date &Date::substractMiliseconds(int miliseconds) { return substractMiliseconds(Milliseconds{miliseconds}); }
    Date &Date::substractTicks(long ticks) { return substract(Microseconds{ticks}); }

    Date &Date::substractYears(Years years) { return substractDate(years); }
    Date &Date::substractMonths(Months months) { return substractDate(months); }
    Date &Date::substractDays(Days days) { return substract(days); }
    Date &Date::substractYears(Year year) { return substractYears(Years{int(year)}); }
    Date &Date::substractMonths(Month month) { return substractMonths(Months{unsigned(month)}); }
    Date &Date::substractDays(Day day) { return substractDays(Days{unsigned(day)}); }
    Date &Date::substractHours(Hours hours) { return substract(hours); }
    Date &Date::substractMinutes(Minutes minutes) { return substract(minutes); }
    Date &Date::substractSeconds(Seconds seconds) { return substract(seconds); }
    Date &Date::substractMiliseconds(Milliseconds miliseconds) { return substract(miliseconds); }

    Date &Date::operator-=(Years years) { return substractYears(years); }
    Date &Date::operator-=(Months months) { return substractMonths(months); }
    Date &Date::operator-=(Days days) { return substractDays(days); }
    Date &Date::operator-=(Year year) { return substractYears(year); }
    Date &Date::operator-=(Month month) { return substractMonths(month); }
    Date &Date::operator-=(Day day) { return substractDays(day); }
    Date &Date::operator-=(Hours hours) { return substractHours(hours); }
    Date &Date::operator-=(Minutes minutes) { return substractMinutes(minutes); }
    Date &Date::operator-=(Seconds seconds) { return substractSeconds(seconds); }
    Date &Date::operator-=(Milliseconds miliseconds) { return substractMiliseconds(miliseconds); }

    Date Date::operator-(Years years) { return Date{*this}.substractYears(years); }
    Date Date::operator-(Months months) { return Date{*this}.substractMonths(months); }
    Date Date::operator-(Days days) { return Date{*this}.substractDays(days); }
    Date Date::operator-(Year year) { return Date{*this}.substractYears(year); }
    Date Date::operator-(Month month) { return Date{*this}.substractMonths(month); }
    Date Date::operator-(Day day) { return Date{*this}.substractDays(day); }
    Date Date::operator-(Hours hours) { return Date{*this}.substractHours(hours); }
    Date Date::operator-(Minutes minutes) { return Date{*this}.substractMinutes(minutes); }
    Date Date::operator-(Seconds seconds) { return Date{*this}.substractSeconds(seconds); }
    Date Date::operator-(Milliseconds miliseconds) { return Date{*this}.substractMiliseconds(miliseconds); }

    template <class Rep, class Period> Date &Date::add(Duration<Rep, Period> duration)
    {
        _timePoint += duration;
        return *this;
    }

    template <class Rep, class Period> Date &Date::addDate(Duration<Rep, Period> duration)
    {
        auto days = getTotalDays();
        auto timeOfDay = _timePoint - days;
        year_month_day ymd{days};
        ymd += duration;
        if (!ymd.ok())
            ymd = ymd.year() / ymd.month() / last;
        _timePoint = sys_days{ymd} + timeOfDay;
        return *this;
    }

    template <class Rep, class Period> Date &Date::substract(Duration<Rep, Period> duration)
    {
        _timePoint -= duration;
        return *this;
    }

    template <class Rep, class Period> Date &Date::substractDate(Duration<Rep, Period> duration)
    {
        auto days = getTotalDays();
        auto timeOfDay = _timePoint - days;
        year_month_day ymd{days};
        ymd -= duration;
        if (!ymd.ok())
            ymd = ymd.year() / ymd.month() / last;
        _timePoint = sys_days{ymd} + timeOfDay;
        return *this;
    }

} // namespace sd