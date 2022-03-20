#include "Datio.hpp"
#include <chrono>
#include <time.h>

using namespace std::chrono;
namespace sd
{

    Date Date::now() { return Date{system_clock::now()}; }
    Date Date::max() { return Date{system_clock::now().max()}; }
    Date Date::min() { return Date{system_clock::now().min()}; }

    Date::Date(time_point<system_clock> timePoint, DateKind dateKind) { _timePoint = timePoint; }

    Date::Date(uint64_t ticks, DateKind dateKind) { _timePoint += microseconds(ticks); }

    Date::Date(int year, unsigned month, unsigned day, DateKind dateKind)
        : Date{Year{year}, Month{month}, Day{day}, dateKind}
    {
    }
    Date::Date(int year, unsigned month, unsigned day, int hour, int minute, int second, DateKind dateKind)
        : Date{Year{year}, Month{month}, Day{day}, Hours{hour}, Minutes{minute}, Seconds{second}, dateKind}
    {
    }
    Date::Date(int year, unsigned month, unsigned day, int hour, int minute, int second, int milisecond,
               DateKind dateKind)
        : Date{Year{year},
               Month{month},
               Day{day},
               Hours{hour},
               Minutes{minute},
               Seconds{second},
               Milliseconds{milisecond},
               dateKind}
    {
    }

    Date::Date(Year year, Month month, Day day, DateKind dateKind) : Date{year, month, day, 0h, 0min, 0s, 0ms, dateKind}
    {
    }
    Date::Date(Year year, Month month, Day day, Hours hour, Minutes minute, Seconds second, DateKind dateKind)
        : Date{year, month, day, hour, minute, second, 0ms, dateKind}
    {
    }
    Date::Date(Year year, Month month, Day day, Hours hour, Minutes minute, Seconds second, Milliseconds milisecond,
               DateKind dateKind)
    {
        year_month_day date{year, month, day};

        system_clock::time_point ld = sys_days(date) + hour + minute + second + milisecond;

        _timePoint = ld;
    }

    Date::Day Date::day()
    {
        auto dp = floor<days>(_timePoint);
        return year_month_day{dp}.day();
    }

    Date::Month Date::month()
    {
        auto dp = floor<days>(_timePoint);
        return year_month_day{dp}.month();
    }

    Date::Year Date::year()
    {
        auto dp = floor<days>(_timePoint);
        return year_month_day{dp}.year();
    }

    std::chrono::weekday Date::dayOfWeek()
    {
        auto dp = floor<days>(_timePoint);
        return {std::chrono::sys_days{year_month_day{dp}}};
    }

    Date::Hours Date::hour()
    {
        auto dp = floor<days>(_timePoint);
        year_month_day ymd{dp};
        hh_mm_ss time{floor<milliseconds>(_timePoint - dp)};
        return time.hours();
    }

    Date::Minutes Date::minute()
    {
        auto dp = floor<days>(_timePoint);
        year_month_day ymd{dp};
        hh_mm_ss time{floor<milliseconds>(_timePoint - dp)};
        return time.minutes();
    }

    Date::Seconds Date::second()
    {
        auto dp = floor<days>(_timePoint);
        year_month_day ymd{dp};
        hh_mm_ss time{floor<milliseconds>(_timePoint - dp)};
        return time.seconds();
    }

    Date::Milliseconds Date::milisecond()
    {
        auto dp = floor<days>(_timePoint);
        year_month_day ymd{dp};
        hh_mm_ss time{floor<milliseconds>(_timePoint - dp)};
        return time.subseconds();
    }
} // namespace sd