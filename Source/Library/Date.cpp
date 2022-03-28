#include "Date.hpp"
#include <chrono>

namespace sd
{
    namespace ch
    {
        using namespace std::chrono;

        using timePoint = time_point<system_clock>;
        using timePointDays = time_point<system_clock, days>;
        using timeZonePtr = const time_zone *;
        using zt = zoned_time<system_clock::duration>;
    } // namespace ch
    namespace
    {
        ch::timeZonePtr getUtcTimeZone()
        {
            static ch::timeZonePtr timeZone = ch::locate_zone("UTC");
            return timeZone;
        }

        ch::timeZonePtr getLocalTimeZone()
        {
            static ch::timeZonePtr timeZone = ch::current_zone();
            return timeZone;
        }

        template <class D> unsigned unsignedCast(D value) { return static_cast<unsigned>(value); }
        auto yearMonthLastDay(int yr, int mo) { return ch::year{yr} / ch::month{unsignedCast(mo)} / ch::last; }

        template <class D> auto microsecondsCast(D duration) { return duration_cast<ch::microseconds>(duration); }
        ch::timePointDays toTotalDays(ch::timePoint timePoint) { return {floor<ch::days>(timePoint)}; }

        Time timeOfDayImpl(ch::timePoint timePoint) { return {microsecondsCast(timePoint - toTotalDays(timePoint))}; }
        ch::year_month_day yearMonthDayImpl(ch::timePoint timePoint) { return {toTotalDays(timePoint)}; }
        ch::year_month_weekday yearMonthWeekdayImpl(ch::timePoint timePoint) { return {toTotalDays(timePoint)}; }

        ch::timePoint createTimePoint(ch::year_month_day yearMonthDay, Time dayTime)
        {
            if (!yearMonthDay.ok())
                yearMonthDay = yearMonthDay.year() / yearMonthDay.month() / ch::last;
            return ch::sys_days{yearMonthDay} + dayTime.raw();
        }

        template <class Rep, class Period>
        ch::timePoint addToTimePoint(ch::timePoint timePoint, ch::duration<Rep, Period> duration)
        {
            if constexpr (ch::duration<Rep, Period>::period::num < ch::months::period::num)
            {
                return timePoint + duration;
            }
            else
            {
                auto days = toTotalDays(timePoint);
                auto ymd = ch::year_month_day{days};
                ch::microseconds timeOfDay = microsecondsCast(timePoint - days);
                ymd += duration;
                return createTimePoint(ymd, timeOfDay);
            }
        }

        std::string toStringRaw(ch::timePoint timepoint) { return std::format("{:L%F %T %Z}", timepoint); }
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
        ch::timePoint timePoint;
        std::stringstream ss{source};
        ch::minutes offset{0};
        std::string abbrev;
        if (from_stream(ss, format.c_str(), timePoint, &abbrev, &offset))
        {
            // ch::timeZonePtr timeZone = nullptr;
            // try
            // {
            //     timeZone = ch::locate_zone(abbrev);
            // }
            // catch (...)
            // {
            //     // timePoint -= offset;
            // }
            date = Date{timePoint};
            return true;
        }
        return false;
    }

    ch::timeZonePtr Date::defaultTimeZone = getLocalTimeZone();

    Date Date::max() { return Date{ch::system_clock::now().max(), Date::defaultTimeZone}; }
    Date Date::min() { return Date{ch::system_clock::now().min(), Date::defaultTimeZone}; }
    Date Date::unixEpoch() { return Date{createTimePoint(ch::year{1970} / ch::January / 1, 0), Date::defaultTimeZone}; }

    Date Date::now() { return Date{ch::system_clock::now(), Date::defaultTimeZone}; }
    Date Date::nowInUtcTimeZone() { return Date{ch::system_clock::now(), getUtcTimeZone()}; }
    Date Date::nowInTimeZone(const std::string &timeZoneName) { return Date{ch::system_clock::now(), timeZoneName}; }
    Date Date::today() { return Date{Date::now().yearMonthDay()}; }

    int Date::daysInMonth(int year, int month) { return Date{yearMonthLastDay(year, month)}.day(); }
    bool Date::isLeapYear(int year) { return Date::daysInMonth(year, 2) == 29; }

    Date::Date(long long microseconds, const std::string &timeZoneName)
        : Date{ch::timePoint{ch::microseconds{microseconds}}, timeZoneName}
    {
    }
    Date::Date(int year, int month, int day, const std::string &timeZoneName)
        : Date{{ch::year{year}, ch::month{unsignedCast(month)}, ch::day{unsignedCast(day)}}, 0, timeZoneName}
    {
    }
    Date::Date(int year, int month, int day, int hour, int minute, int second, const std::string &timeZoneName)
        : Date{{ch::year{year}, ch::month{unsignedCast(month)}, ch::day{unsignedCast(day)}},
               ch::hours{hour} + ch::minutes{minute} + ch::seconds{second},
               timeZoneName}
    {
    }
    Date::Date(int year, int month, int day, int hour, int minute, int second, int milisecond,
               const std::string &timeZoneName)
        : Date{{ch::year{year}, ch::month{unsignedCast(month)}, ch::day{unsignedCast(day)}},
               ch::hours{hour} + ch::minutes{minute} + ch::seconds{second} + ch::milliseconds{milisecond},
               timeZoneName}
    {
    }
    Date::Date(ch::year_month_day date, Time timeOfDay, const std::string &timeZoneName)
        : Date{createTimePoint(date, timeOfDay), timeZoneName, true}
    {
    }
    Date::Date(ch::timePoint timePoint, const std::string &timeZoneName, bool normalize)
        : Date{timePoint, timeZoneName.empty() ? nullptr : ch::locate_zone(timeZoneName), normalize}
    {
    }

    Date::Date(ch::timePoint timePoint, ch::timeZonePtr timeZone, bool normalize)
        : _timePoint{timePoint}, _timeZone{!timeZone ? Date::defaultTimeZone : timeZone}
    {
        if (!_timeZone)
            throw std::invalid_argument("Error while creating Date, time zone cannot be nullptr");
        auto originalInfo = _timeZone->get_info(_timePoint);
        _offset = originalInfo.offset;

        if (!normalize)
            return;

        _timePoint -= originalInfo.offset;
        // swallow ambigious dates on purpuse
        if (_timePoint < originalInfo.begin || _timePoint >= originalInfo.end)
        {
            auto borderInfo = _timeZone->get_info(_timePoint);
            _offset = borderInfo.offset;
            _timePoint = _timePoint + originalInfo.offset - borderInfo.offset;
            if (_timePoint >= originalInfo.begin && _timePoint < originalInfo.end)
            {
                throw std::invalid_argument("Error while creating Date, date in specified time zone doesnt exisits");
            }
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

    Time Date::timeOfDay() const { return timeOfDayImpl(zonedTimePoint()); }
    int Date::weekOfMonth() const { return yearMonthWeekday().weekday_indexed().index(); }
    DayOfWeek Date::dayOfWeek() const { return static_cast<DayOfWeek>(yearMonthWeekday().weekday().iso_encoding()); }
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

    ch::year_month_day Date::yearMonthDay() const { return yearMonthDayImpl(zonedTimePoint()); }
    ch::year_month_weekday Date::yearMonthWeekday() const { return yearMonthWeekdayImpl(zonedTimePoint()); }

    ch::timePoint Date::timePoint() const { return _timePoint; }
    ch::timePoint Date::zonedTimePoint() const { return _timePoint + offset(); }
    ch::seconds Date::offset() const { return _offset; }
    ch::timeZonePtr Date::timeZone() const { return _timeZone; };

    std::string Date::toString(const std::string &f) const { return std::format(f, ch::zt{timeZone(), timePoint()}); }

    Date Date::toUtcTimeZone() const { return Date{*this}.changeTimeZoneToUtc(); }
    Date Date::toLocalTimeZone() const { return Date{*this}.changeTimeZoneToLocal(); }
    Date Date::toDefaultTimeZone() const { return Date{*this}.changeTimeZoneToDefault(); }
    Date Date::toTimeZone(const std::string &tzName) const { return Date{*this}.changeTimeZone(tzName); }
    Date Date::toTimeZone(ch::timeZonePtr timeZone) const { return Date{*this}.changeTimeZone(timeZone); }

    void Date::recomputeOffset() { _offset = timeZone()->get_info(timePoint()).offset; }

    Date &Date::changeTimeZoneToUtc() { return changeTimeZone(getUtcTimeZone()); }
    Date &Date::changeTimeZoneToLocal() { return changeTimeZone(getLocalTimeZone()); }
    Date &Date::changeTimeZoneToDefault() { return changeTimeZone(Date::defaultTimeZone); }
    Date &Date::changeTimeZone(const std::string &tzName) { return changeTimeZone(ch::locate_zone(tzName)); }
    Date &Date::changeTimeZone(ch::timeZonePtr timeZone)
    {
        _timeZone = timeZone;
        recomputeOffset();
        return *this;
    }

    bool Date::isDaylightSavingTime() { return timeZone()->get_info(timePoint()).save > 0; }

    Date &Date::addYears(int years) { return add(ch::years{years}); }
    Date &Date::addMonths(int months) { return add(ch::months{months}); }
    Date &Date::addDays(int days) { return add(Time::fromDays(days)); }
    Date &Date::addHours(int hours) { return add(Time::fromHours(hours)); }
    Date &Date::addMinutes(int minutes) { return add(Time::fromMinutes(minutes)); }
    Date &Date::addSeconds(int seconds) { return add(Time::fromSeconds(seconds)); }
    Date &Date::addMiliseconds(int miliseconds) { return add(Time::fromMiliseconds(miliseconds)); }
    Date &Date::addMicroseconds(long long microseconds) { return add(Time::fromMicroseconds(microseconds)); }
    Date &Date::add(const Time &time) { return add(time.raw()); }
    template <class Rep, class Period> Date &Date::add(ch::duration<Rep, Period> duration)
    {
        _timePoint = addToTimePoint(_timePoint, duration);
        recomputeOffset();
        return *this;
    }

    Date &Date::substractYears(int years) { return addYears(-years); }
    Date &Date::substractMonths(int months) { return addMonths(-months); }
    Date &Date::substractDays(int days) { return addDays(-days); }
    Date &Date::substractHours(int hours) { return addHours(-hours); }
    Date &Date::substractMinutes(int minutes) { return addMinutes(-minutes); }
    Date &Date::substractSeconds(int seconds) { return addSeconds(-seconds); }
    Date &Date::substractMiliseconds(int miliseconds) { return addMiliseconds(-miliseconds); }
    Date &Date::substractMicroseconds(long long microseconds) { return addMicroseconds(-microseconds); }
    Date &Date::substract(const Time &time) { return add(-time); }
    Time Date::substract(const Date &date) const { return Time{microsecondsCast(timePoint() - date.timePoint())}; }

    Date &Date::operator+=(const Time &time) { return add(time); }
    Date &Date::operator+=(const Years &years) { return addYears(years.value); }
    Date &Date::operator+=(const Months &months) { return addMonths(months.value); }

    Date &Date::operator-=(const Time &time) { return substract(time); }
    Date &Date::operator-=(const Years &years) { return substractYears(years.value); }
    Date &Date::operator-=(const Months &months) { return substractMonths(months.value); }

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

    Date::operator bool() const { return timePoint() != 0; }

    Years operator"" _y(unsigned long long years) { return Years(static_cast<int>(years)); }
    Months operator"" _mo(unsigned long long months) { return Months(static_cast<int>(months)); }

} // namespace sd