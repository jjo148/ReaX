#pragma once

struct Scheduler::Impl
{
    typedef std::function<rxcpp::observable<juce::var>(const rxcpp::observable<juce::var>&)> Schedule;

    Impl(const Schedule& schedule);

    const Schedule schedule;
};
