#pragma once

namespace detail {
struct SchedulerImpl
{
    typedef std::function<rxcpp::observable<any>(const rxcpp::observable<any>&)> Schedule;

    SchedulerImpl(const Schedule& schedule);

    const Schedule schedule;
};
}
