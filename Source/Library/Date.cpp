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
    using ZT = std::chrono::zoned_time<std::chrono::system_clock::duration>;
    using TimeZonePtr = const std::chrono::time_zone *;
    template <class Rep, class Period = std::ratio<1>> using Dur = std::chrono::duration<Rep, Period>;

    using TimePoint = std::chrono::time_point<std::chrono::system_clock>;
    using TimePointDays = std::chrono::time_point<std::chrono::system_clock, std::chrono::days>;

    using namespace std::chrono;
    namespace
    {
        TimeZonePtr getUtcTimeZone()
        {
            static TimeZonePtr timeZone = locate_zone("UTC");
            return timeZone;
        }

        TimeZonePtr getLocalTimeZone()
        {
            static TimeZonePtr timeZone = current_zone();
            return timeZone;
        }

        template <class D> auto castToMicroseconds(D dur) { return duration_cast<Microseconds>(dur); }

        TimePointDays toTotalDays(TimePoint timePoint) { return floor<CDays>(timePoint); }

        auto YMWD(TimePoint timePoint) { return YearMonthWeekday{toTotalDays(timePoint)}; }

        TimePoint createTimePoint(YearMonthDay ymd, Time dayTime)
        {
            if (!ymd.ok())
                ymd = ymd.year() / ymd.month() / last;
            return sys_days{ymd} + dayTime.raw();
        }

        auto decomposeTimePoint(TimePoint timePoint)
        {
            auto days = toTotalDays(timePoint);
            Microseconds rest = castToMicroseconds(timePoint - days);
            return std::make_pair(YearMonthDay{days}, rest);
        }

    } // namespace

    Date Date::parse(const std::string &source, const std::string &format)
    {
        Date date{0};
        if (!tryParse(date, source, format))
            throw std::runtime_error(std::format("Invalid date format: '{}' for input: '{}'", format, source));
        return date;
    }

    bool Date::tryParse(Date &date, const std::string &source, const std::string &format)
    {
        TimePoint timePoint;
        std::stringstream ss{source};
        Minutes offset{0};
        std::string abbrev;
        if (from_stream(ss, format.c_str(), timePoint, &abbrev, &offset))
        {
            TimeZonePtr timeZone = nullptr;
            try
            {
                timeZone = locate_zone(abbrev);
            }
            catch (...)
            {
                timePoint -= offset;
            }
            date = Date{timePoint, timeZone, true};
            return true;
        }
        return false;
    }

    TimeZonePtr Date::defaultTimeZone = getLocalTimeZone();

    Date Date::max() { return Date{system_clock::now().max(), Date::defaultTimeZone}; }
    Date Date::min() { return Date{system_clock::now().min(), Date::defaultTimeZone}; }
    Date Date::unixEpoch() { return Date{createTimePoint(1970y / January / 1, 0), Date::defaultTimeZone}; }

    Date Date::now() { return Date{system_clock::now(), Date::defaultTimeZone}; }
    Date Date::nowInUtcTimeZone() { return Date{system_clock::now(), getUtcTimeZone()}; }
    Date Date::nowInTimeZone(const std::string &timeZoneName) { return Date{system_clock::now(), timeZoneName}; }
    Date Date::today() { return Date{Date::now().yearMonthDay()}; }

    int Date::daysInMonth(int year, int month)
    {
        return Date{{Year{year} / Month{static_cast<unsigned>(month)} / last}}.day();
    }
    bool Date::isLeapYear(int year) { return Date::daysInMonth(year, 2) == 29; }

    Date::Date(long long microseconds, const std::string &timeZoneName)
        : Date{TimePoint{Microseconds{microseconds}}, timeZoneName}
    {
    }
    Date::Date(int year, int month, int day, const std::string &timeZoneName)
        : Date{{Year{year}, Month{static_cast<unsigned>(month)}, Day{static_cast<unsigned>(day)}}, 0, timeZoneName}
    {
    }
    Date::Date(int year, int month, int day, int hour, int minute, int second, int milisecond,
               const std::string &timeZoneName)
        : Date{{Year{year}, Month{static_cast<unsigned>(month)}, Day{static_cast<unsigned>(day)}},
               Hours{hour} + Minutes{minute} + Seconds{second} + Milliseconds{milisecond},
               timeZoneName}
    {
    }
    Date::Date(YearMonthDay date, Time timeOfDay, const std::string &timeZoneName)
        : Date{createTimePoint(date, timeOfDay), timeZoneName, true}
    {
    }
    Date::Date(TimePoint timePoint, const std::string &timeZoneName, bool normalize)
        : Date{timePoint, timeZoneName.empty() ? nullptr : locate_zone(timeZoneName), normalize}
    {
    }

    Date::Date(TimePoint timePoint, TimeZonePtr timeZone, bool normalize)
    {
        _timePoint = timePoint;
        _timeZone = !timeZone ? Date::defaultTimeZone : timeZone;
        auto info = _timeZone->get_info(_timePoint);
        _offset = info.offset;

        if (!normalize)
            return;

        _timePoint -= info.offset;
        if (_timePoint < info.begin)
        {
            auto newInfo = _timeZone->get_info(_timePoint);
            _timePoint = _timePoint + info.offset - newInfo.offset;
            _offset = newInfo.offset;
        }
    }

    int Date::year() const { return int{yearMonthDay().year()}; }
    int Date::day() const { return unsigned{yearMonthDay().day()}; }
    int Date::month() const { return unsigned{yearMonthDay().month()}; }
    int Date::hour() const { return timeOfDay().hours(); }
    int Date::minute() const { return timeOfDay().minutes(); }
    int Date::second() const { return timeOfDay().seconds(); }
    int Date::milisecond() const { return timeOfDay().miliseconds(); }
    long long Date::microsecond() const { return timeOfDay().microseconds(); }

    Time Date::timeOfDay() const { return Time{decomposeTimePoint(zonedTimePoint()).second}; }
    int Date::weekOfMonth() const { return YMWD(zonedTimePoint()).weekday_indexed().index(); }
    DayOfWeek Date::dayOfWeek() const
    {
        return static_cast<DayOfWeek>(YMWD(zonedTimePoint()).weekday().iso_encoding());
    }
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

    YearMonthDay Date::yearMonthDay() const { return decomposeTimePoint(zonedTimePoint()).first; }

    TimePoint Date::timePoint() const { return _timePoint; }
    TimePoint Date::zonedTimePoint() const { return _timePoint + offset(); }
    Seconds Date::offset() const { return _offset; }
    TimeZonePtr Date::timeZone() const { return _timeZone; };

    std::string Date::toString(const std::string &fmt) const { return std::format(fmt, ZT{timeZone(), timePoint()}); }

    Date Date::toUtcTimeZone() const { return Date{*this}.changeTimeZoneToUtc(); }
    Date Date::toLocalTimeZone() const { return Date{*this}.changeTimeZoneToLocal(); }
    Date Date::toDefaultTimeZone() const { return Date{*this}.changeTimeZoneToDefault(); }
    Date Date::toTimeZone(const std::string &tzName) const { return Date{*this}.changeTimeZone(tzName); }
    Date Date::toTimeZone(TimeZonePtr timeZone) const { return Date{*this}.changeTimeZone(timeZone); }

    Date &Date::changeTimeZoneToUtc() { return changeTimeZone(getUtcTimeZone()); }
    Date &Date::changeTimeZoneToLocal() { return changeTimeZone(getLocalTimeZone()); }
    Date &Date::changeTimeZoneToDefault() { return changeTimeZone(Date::defaultTimeZone); }
    Date &Date::changeTimeZone(const std::string &tzName) { return changeTimeZone(locate_zone(tzName)); }
    Date &Date::changeTimeZone(TimeZonePtr timeZone)
    {
        _timeZone = timeZone;
        recomputeOffset();
        return *this;
    }

    bool Date::isDaylightSavingTime() { return timeZone()->get_info(timePoint()).save > 0; }

    Date &Date::add(const Time &time) { return add(time.raw()); }
    Date &Date::addYears(int years) { return add(CYears{years}); }
    Date &Date::addMonths(int months) { return add(CMonths{months}); }

    template <class Rep, class Period> Date &Date::add(Dur<Rep, Period> duration)
    {
        if constexpr (Dur<Rep, Period>::period::num < CMonths::period::num)
        {
            _timePoint += duration;
        }
        else
        {
            auto [ymd, timeOfDay] = decomposeTimePoint(timePoint());
            ymd += duration;
            _timePoint = createTimePoint(ymd, timeOfDay);
        }
        recomputeOffset();
        return *this;
    }

    Date &Date::addDays(int days) { return add(Time::fromDays(days)); }
    Date &Date::addHours(int hours) { return add(Time::fromHours(hours)); }
    Date &Date::addMinutes(int minutes) { return add(Time::fromMinutes(minutes)); }
    Date &Date::addSeconds(int seconds) { return add(Time::fromSeconds(seconds)); }
    Date &Date::addMiliseconds(int miliseconds) { return add(Time::fromMiliseconds(miliseconds)); }
    Date &Date::addMicroseconds(long long microseconds) { return add(Time::fromMicroseconds(microseconds)); }

    Time Date::substract(const Date &date) const { return Time{castToMicroseconds(timePoint() - date.timePoint())}; }
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

    void Date::recomputeOffset() { _offset = timeZone()->get_info(timePoint()).offset; }

    Date operator+(const Date &date, const Time &time) { return Date{date}.add(time); }
    Date operator+(const Date &date, const Years &years) { return Date{date}.addYears(years.value); }
    Date operator+(const Date &date, const Months &months) { return Date{date}.addMonths(months.value); }

    Date operator-(const Date &date, const Time &time) { return Date{date}.substract(time); }
    Date operator-(const Date &date, const Years &years) { return Date{date}.substractYears(years.value); }
    Date operator-(const Date &date, const Months &months) { return Date{date}.substractMonths(months.value); }

    Time operator-(const Date &lhs, const Date &rhs) { return Date{lhs}.substract(rhs); }

    bool operator==(const Date &lhs, const Date &rhs) { return lhs.timePoint() == rhs.timePoint(); }
    bool operator!=(const Date &lhs, const Date &rhs) { return lhs.timePoint() != rhs.timePoint(); }
    bool operator<(const Date &lhs, const Date &rhs) { return lhs.timePoint() < rhs.timePoint(); }
    bool operator<=(const Date &lhs, const Date &rhs) { return lhs.timePoint() <= rhs.timePoint(); }
    bool operator>(const Date &lhs, const Date &rhs) { return lhs.timePoint() > rhs.timePoint(); }
    bool operator>=(const Date &lhs, const Date &rhs) { return lhs.timePoint() >= rhs.timePoint(); }

    Years operator"" _y(unsigned long long years) { return Years(static_cast<int>(years)); }
    Months operator"" _mo(unsigned long long months) { return Months(static_cast<int>(months)); }

} // namespace sd