#include "Datio.hpp"
#include <chrono>

namespace sd
{
    using namespace std::chrono;
    using namespace Time;
    namespace
    {
        template <class Rep, class Period> TimePoint createTimePoint(YearMontDay ymd, Duration<Rep, Period> dayTime)
        {
            if (!ymd.ok())
                ymd = ymd.year() / ymd.month() / last;
            return sys_days{ymd} + dayTime;
        }

        TimePointDays readTotalDays(TimePoint timePoint) { return floor<Days>(timePoint); }

        auto decomposeTimePoint(TimePoint timePoint)
        {
            auto days = readTotalDays(timePoint);
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
        _timePoint = createTimePoint({year, month, day}, hour + minute + second + milisecond);
    }

    Day Date::day() const { return getYearMonthDay().day(); }
    Month Date::month() const { return getYearMonthDay().month(); }
    Year Date::year() const { return getYearMonthDay().year(); }
    WeakDay Date::dayOfWeek() const { return {sys_days{getYearMonthDay()}}; }

    Hours Date::hour() const { return getHHMMSS().hours(); }
    Minutes Date::minute() const { return getHHMMSS().minutes(); }
    Seconds Date::second() const { return getHHMMSS().seconds(); }
    Milliseconds Date::milisecond() const { return getHHMMSS().subseconds(); }

    long Date::ticks() const { return static_cast<long>(floor<Microseconds>(_timePoint).time_since_epoch().count()); }

    YearMontDay Date::getYearMonthDay() const { return readYearMonthDay(_timePoint); }

    HHMMSS Date::getHHMMSS() const { return readHMMSS(_timePoint); }

    Date &Date::addYears(int years) { return addYears(Years{years}); }
    Date &Date::addMonths(int months) { return addMonths(Months{months}); }
    Date &Date::addDays(int days) { return addDays(Days{days}); }
    Date &Date::addHours(int hours) { return addHours(Hours{hours}); }
    Date &Date::addMinutes(int minutes) { return addMinutes(Minutes{minutes}); }
    Date &Date::addSeconds(int seconds) { return addSeconds(Seconds{seconds}); }
    Date &Date::addMiliseconds(int miliseconds) { return addMiliseconds(Milliseconds{miliseconds}); }
    Date &Date::addTicks(long ticks) { return add(Microseconds{ticks}); }

    Date &Date::addYears(Years years) { return add(years); }
    Date &Date::addMonths(Months months) { return add(months); }
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
    Date &Date::substractTicks(long ticks) { return add(Microseconds{ticks}); }

    Date &Date::substractYears(Years years) { return add(-years); }
    Date &Date::substractMonths(Months months) { return add(-months); }
    Date &Date::substractDays(Days days) { return add(-days); }
    Date &Date::substractYears(Year year) { return add(Years{-int(year)}); }
    Date &Date::substractMonths(Month month) { return add(Months{-unsigned(month)}); }
    Date &Date::substractDays(Day day) { return add(Days{-unsigned(day)}); }
    Date &Date::substractHours(Hours hours) { return add(-hours); }
    Date &Date::substractMinutes(Minutes minutes) { return add(-minutes); }
    Date &Date::substractSeconds(Seconds seconds) { return add(-seconds); }
    Date &Date::substractMiliseconds(Milliseconds miliseconds) { return add(-miliseconds); }

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
        if constexpr (Duration<Rep, Period>::period::num < Months::period::num)
        {
            _timePoint += duration;
        }
        else
        {
            auto [ymd, timeOfDay] = decomposeTimePoint(_timePoint);
            ymd += duration;
            _timePoint = createTimePoint(ymd, timeOfDay);
        }
        return *this;
    }

} // namespace sd