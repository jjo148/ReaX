#pragma once

namespace detail {
    struct SchedulerImpl;
}

/**
    A Scheduler is used to process parts of an Observable on a specific thread.
 
    Use the Scheduler::messageThread, Scheduler::backgroundThread and Scheduler::newThread member functions and pass the returned Scheduler to Observable::observeOn.
 
    @see Observable::observeOn
 */
class Scheduler
{
public:
    /// The JUCE message thread. 
    static Scheduler messageThread();

    /// A shared background thread. Use this if you don't want to block the message thread, but don't want to spawn a new thread either. The thread is shared between Observables. 
    static Scheduler backgroundThread();

    /// Makes the Observable spawn a new thread. 
    static Scheduler newThread();

private:
    template<typename T>
    friend class Observable;
    
    std::shared_ptr<detail::SchedulerImpl> impl;
    Scheduler(const std::shared_ptr<detail::SchedulerImpl>&);

    JUCE_LEAK_DETECTOR(Scheduler)
};
