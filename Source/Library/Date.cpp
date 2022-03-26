#include "Date.hpp"
#include <chrono>

namespace sd
{
    using Year = std::chrono::year;
    using Month = std::chrono::month;
    using Day = std::chrono::day;
    using CYears = std::chrono::years;
    using CMonths = std::chrono::months;
    using CDays = std::chrono::days;
    using Hours = std::chrono::hours;
    using Minutes = std::chrono::minutes;
    using Seconds = std::chrono::seconds;
    using Milliseconds = std::chrono::milliseconds;
    using Microseconds = std::chrono::microseconds;
    using YearMonthDay = std::chrono::year_month_day;
    using YearMonthWeekday = std::chrono::year_month_weekday;
    using ZonedTime = std::chrono::zoned_time<std::chrono::system_clock::duration>;

    using TimePoint = std::chrono::time_point<std::chrono::system_clock>;
    using TimePointDays = std::chrono::time_point<std::chrono::system_clock, std::chrono::days>;

    using namespace std::chrono;
    namespace
    {

        template <class D> auto castToMicroseconds(D dur) { return duration_cast<Microseconds>(dur); }

        auto getOffset(DateKind dateKind){return dateKind == DateKind::Local ? }

        TimePointDays toTotalDays(TimePoint timePoint, DateKind dateKind)
        {
            return floor<CDays>(timePoint);
        }

        TimePoint createTimePoint(YearMonthDay ymd, Time dayTime)
        {
            if (!ymd.ok())
                ymd = ymd.year() / ymd.month() / last;
            return sys_days{ymd} + dayTime.raw();
        }

        ZonedTime createZonedTime(YearMonthDay ymd, Time dayTime, DateKind dateKind = Date::defaultDateKind)
        {
            auto timepoint = createTimePoint(ymd, dayTime);
            auto curr = get_tzdb().current_zone();
            if (dateKind == DateKind::Local)
            {
                auto info = curr->get_info(timepoint);
                timepoint = timepoint - info.offset;
            }
            return ZonedTime{curr, timepoint};
        }

        auto decomposeTimePoint(TimePoint timePoint)
        {
            auto days = toTotalDays(timePoint);
            Microseconds rest = castToMicroseconds(timePoint - days);
            return std::make_pair(YearMonthDay{days}, rest);
        }

        auto YMWD(TimePoint timePoint) { return YearMonthWeekday{toTotalDays(timePoint)}; }

    } // namespace

    Date Date::parse(const std::string &source, const std::string &format)
    {
        TimePoint timePoint;
        std::stringstream ss{source};
        from_stream(ss, format.c_str(), timePoint);
        return Date{timePoint};
    }

    DateKind Date::defaultDateKind = DateKind::Local;

    const Date Date::max = Date{system_clock::now().max()};
    const Date Date::min = Date{system_clock::now().min()};
    const Date Date::unixEpoch = Date{1970, 1, 1};

    Date Date::now() { return Date{system_clock::now()}; }
    Date Date::utcNow() { return Date::now(); }
    Date Date::today() { return Date{Date::now().yearMonthDay()}; }

    int Date::daysInMonth(int year, int month)
    {
        return Date{{Year{year} / Month{static_cast<unsigned>(month)} / last}}.day();
    }
    bool Date::isLeapYear(int year) { return Date::daysInMonth(year, 2) == 29; }

    Date::Date(long long microseconds) { add(Microseconds{microseconds}); }
    Date::Date(int year, int month, int day, int hour, int minute, int second, int milisecond)
        : Date{{Year{year}, Month{static_cast<unsigned>(month)}, Day{static_cast<unsigned>(day)}},
               Hours{hour} + Minutes{minute} + Seconds{second} + Milliseconds{milisecond}}
    {
    }
    Date::Date(YearMonthDay date, Time timeOfDay) : Date{createTimePoint(date, timeOfDay)} {}
    Date::Date(TimePoint timePoint)
    {
        _timePoint = timePoint;
        auto curr = get_tzdb().current_zone();
        _zonedTime = {curr, timePoint};

        auto s = _zonedTime.get_sys_time();
        auto l = _zonedTime.get_local_time();

        auto zzz = _zonedTime.get_info();
        auto [begin, end, offset, save, abbrev] = zzz;
        auto gg = decomposeTimePoint(s);
        auto t1 = std::chrono::hh_mm_ss{gg.second};
        auto gg2 = decomposeTimePoint(_timePoint);
        auto t2 = std::chrono::hh_mm_ss{gg2.second};
        auto hh = 10;
    }

    int Date::year() const { return int{yearMonthDay().year()}; }
    int Date::day() const { return unsigned{yearMonthDay().day()}; }
    int Date::month() const { return unsigned{yearMonthDay().month()}; }
    int Date::hour() const { return timeOfDay().hours(); }
    int Date::minute() const { return timeOfDay().minutes(); }
    int Date::second() const { return timeOfDay().seconds(); }
    int Date::milisecond() const { return timeOfDay().miliseconds(); }
    long long Date::microsecond() const { return timeOfDay().microseconds(); }

    Time Date::timeOfDay() const { return Time{decomposeTimePoint(raw()).second}; }
    int Date::weekOfMonth() const { return YMWD(raw()).weekday_indexed().index(); }
    DayOfWeek Date::dayOfWeek() const { return static_cast<DayOfWeek>(YMWD(raw()).weekday().iso_encoding()); }
    int Date::dayOfYear() const
    {
        // todo optimize this code
        int result = 0, y = year(), m = month();
        for (int i = 1; i < m; ++i)
        {
            result += Date::daysInMonth(y, i);
        }
        return result + day();
    }

    YearMonthDay Date::yearMonthDay() const { return decomposeTimePoint(raw()).first; }

    TimePoint Date::raw() const { return _timePoint; }

    std::string Date::toString(const std::string &format) const { return std::format(format, raw()); }

    Date &Date::add(const Time &time)
    {
        _timePoint += time.raw();
        return *this;
    }
    Date &Date::addYears(int years)
    {
        auto [ymd, timeOfDay] = decomposeTimePoint(raw());
        ymd += CYears{years};
        _timePoint = createTimePoint(ymd, castToMicroseconds(timeOfDay));
        return *this;
    }
    Date &Date::addMonths(int months)
    {
        auto [ymd, timeOfDay] = decomposeTimePoint(raw());
        ymd += CMonths{months};
        _timePoint = createTimePoint(ymd, castToMicroseconds(timeOfDay));
        return *this;
    }
    Date &Date::addDays(int days) { return add(Time::fromDays(days)); }
    Date &Date::addHours(int hours) { return add(Time::fromHours(hours)); }
    Date &Date::addMinutes(int minutes) { return add(Time::fromMinutes(minutes)); }
    Date &Date::addSeconds(int seconds) { return add(Time::fromSeconds(seconds)); }
    Date &Date::addMiliseconds(int miliseconds) { return add(Time::fromMiliseconds(miliseconds)); }
    Date &Date::addMicroseconds(long long microseconds) { return add(Time::fromMicroseconds(microseconds)); }

    Time Date::substract(const Date &date) const { return Time{castToMicroseconds(raw() - date.raw())}; }
    Date &Date::substract(const Time &time) { return add(-time); }
    Date &Date::substractYears(int years) { return addYears(-years); }
    Date &Date::substractMonths(int months) { return addMonths(-months); }
    Date &Date::substractDays(int days) { return addDays(-days); }
    Date &Date::substractHours(int hours) { return addHours(-hours); }
    Date &Date::substractMinutes(int minutes) { return addMinutes(-minutes); }
    Date &Date::substractSeconds(int seconds) { return addSeconds(-seconds); }
    Date &Date::substractMiliseconds(int miliseconds) { return addMiliseconds(-miliseconds); }
    Date &Date::substractMicroseconds(long long microseconds) { return addMicroseconds(-microseconds); }

    Date &Date::operator+=(const Time &time) { return add(time); }
    Date &Date::operator+=(const Years &years) { return addYears(years.value); }
    Date &Date::operator+=(const Months &months) { return addMonths(months.value); }

    Date &Date::operator-=(const Time &time) { return substract(time); }
    Date &Date::operator-=(const Years &years) { return substractYears(years.value); }
    Date &Date::operator-=(const Months &months) { return substractMonths(months.value); }

    Date::operator bool() const { return *this != Date{0}; }

    Date operator+(const Date &date, const Time &time) { return Date{date}.add(time); }
    Date operator+(const Date &date, const Years &years) { return Date{date}.addYears(years.value); }
    Date operator+(const Date &date, const Months &months) { return Date{date}.addMonths(months.value); }

    Date operator-(const Date &date, const Time &time) { return Date{date}.substract(time); }
    Date operator-(const Date &date, const Years &years) { return Date{date}.substractYears(years.value); }
    Date operator-(const Date &date, const Months &months) { return Date{date}.substractMonths(months.value); }

    Time operator-(const Date &lhs, const Date &rhs) { return Date{lhs}.substract(rhs); }

    bool operator==(const Date &lhs, const Date &rhs) { return lhs.raw() == rhs.raw(); }
    bool operator!=(const Date &lhs, const Date &rhs) { return lhs.raw() != rhs.raw(); }
    bool operator<(const Date &lhs, const Date &rhs) { return lhs.raw() < rhs.raw(); }
    bool operator<=(const Date &lhs, const Date &rhs) { return lhs.raw() <= rhs.raw(); }
    bool operator>(const Date &lhs, const Date &rhs) { return lhs.raw() > rhs.raw(); }
    bool operator>=(const Date &lhs, const Date &rhs) { return lhs.raw() >= rhs.raw(); }

    Years operator"" _y(unsigned long long years) { return Years(static_cast<int>(years)); }
    Months operator"" _mo(unsigned long long months) { return Months(static_cast<int>(months)); }

} // namespace sd