namespace
{
    using namespace juce;

    // A Rx dispatcher for the JUCE message thread. It processes Observables that are observed on it.
    class JUCEDispatcher  : private Timer
    {
    public:
        JUCEDispatcher()
          : runLoop (createRunLoop())
        {
            // The run loop didn't get initialized! Please report this as a bug.
            jassert (runLoop);

            startTimerHz (60);
        }

        rxcpp::observe_on_one_worker createWorker() const
        {
            return rxcpp::observe_on_run_loop (*runLoop);
        }

    private:
        typedef std::unique_ptr<rxcpp::schedulers::run_loop> RunLoop_ptr;
        const RunLoop_ptr runLoop;

        static RunLoop_ptr createRunLoop()
        {
            // Create the run loop on the message thread:
            RunLoop_ptr rl;
            static const auto create = [] (void* rl) {
                // Not called from the JUCE message thread! Please report this as a bug.
                jassert (MessageManager::getInstance()->isThisTheMessageThread());

                *static_cast<RunLoop_ptr*> (rl) = std::make_unique<rxcpp::schedulers::run_loop>();
                return static_cast<void*> (nullptr);
            };

            // This will block if this thread isn't the message thread:
            MessageManager::getInstance()->callFunctionOnMessageThread (create, &rl);

            return rl;
        }

        void timerCallback() override
        {
            // Run any scheduled actions
            while (!runLoop->empty() && runLoop->peek().when < runLoop->now())
                runLoop->dispatch();
        }
    };
} // namespace

Scheduler::Scheduler (const std::shared_ptr<detail::SchedulerImpl>& impl)
  : impl (impl) {}

Scheduler Scheduler::messageThread()
{
    static const JUCEDispatcher dispatcher;
    const auto worker = dispatcher.createWorker();
    return std::make_shared<detail::SchedulerImpl> ([worker] (const rxcpp::observable<detail::any>& observable) {
        return observable.observe_on (worker);
    });
}

Scheduler Scheduler::backgroundThread()
{
    return std::make_shared<detail::SchedulerImpl> ([] (const rxcpp::observable<detail::any>& observable) {
        return observable.observe_on (rxcpp::serialize_event_loop());
    });
}

Scheduler Scheduler::newThread()
{
    return std::make_shared<detail::SchedulerImpl> ([] (const rxcpp::observable<detail::any>& observable) {
        return observable.observe_on (rxcpp::serialize_new_thread());
    });
}
