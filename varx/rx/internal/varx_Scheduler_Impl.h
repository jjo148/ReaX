#pragma once

struct Scheduler::Impl
{
    typedef std::function<rxcpp::observable<detail::any>(const rxcpp::observable<detail::any>&)> Schedule;

    Impl(const Schedule& schedule);

    const Schedule schedule;
};
