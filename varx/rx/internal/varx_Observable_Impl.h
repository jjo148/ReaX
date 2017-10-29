#pragma once

class Scheduler;

namespace detail {
struct ObserverImpl;
struct SchedulerImpl;

struct ObservableImpl
{
    ObservableImpl(const any& wrapped);

    // Creation
    static ObservableImpl create(const std::function<void(ObserverImpl&&)>& onSubscribe);
    static ObservableImpl defer(const std::function<ObservableImpl()>& factory);
    static ObservableImpl empty();
    static ObservableImpl error(const std::exception& error);
    static ObservableImpl from(juce::Array<any>&& items);
    static ObservableImpl fromValue(juce::Value value);
    static ObservableImpl interval(const juce::RelativeTime& interval);
    static ObservableImpl just(const any& value);
    static ObservableImpl never();
    static ObservableImpl integralRange(long long first, long long last, unsigned int step);
    static ObservableImpl floatRange(float first, float last, unsigned int step);
    static ObservableImpl doubleRange(double first, double last, unsigned int step);
    static ObservableImpl repeat(const any& item);
    static ObservableImpl repeat(const any& item, unsigned int times);

    // Subscription
    Subscription subscribe(const std::function<void(const any&)>& onNext,
                         const std::function<void(std::exception_ptr)>& onError,
                         const std::function<void()>& onCompleted) const;
    Subscription subscribe(const ObserverImpl& observer) const;

    // Operators
    ObservableImpl combineLatest(std::initializer_list<ObservableImpl> others, const any& transform) const;
    ObservableImpl concat(const juce::Array<ObservableImpl>& others) const;
    ObservableImpl debounce(const juce::RelativeTime& interval) const;
    ObservableImpl distinctUntilChanged(const std::function<bool(const any&, const any&)>& equals) const;
    ObservableImpl elementAt(int index) const;
    ObservableImpl filter(const std::function<bool(const any&)>& predicate) const;
    ObservableImpl flatMap(const std::function<ObservableImpl(const any&)>& transform) const;
    ObservableImpl map(const std::function<any(const any&)>& transform) const;
    ObservableImpl merge(const juce::Array<ObservableImpl>& others) const;
    ObservableImpl reduce(const any& startValue, const std::function<any(const any&, const any&)>& f) const;
    ObservableImpl sample(const juce::RelativeTime& interval) const;
    ObservableImpl scan(const any& startValue, const std::function<any(const any&, const any&)>& f) const;
    ObservableImpl skip(unsigned int numItems) const;
    ObservableImpl skipUntil(const ObservableImpl& other) const;
    ObservableImpl startWith(juce::Array<any>&& items) const;
    ObservableImpl switchOnNext() const;
    ObservableImpl take(unsigned int numItems) const;
    ObservableImpl takeLast(unsigned int numItems) const;
    ObservableImpl takeUntil(const ObservableImpl& other) const;
    ObservableImpl takeWhile(const std::function<bool(const any&)>& predicate) const;
    ObservableImpl withLatestFrom(std::initializer_list<ObservableImpl> others, const any& transform) const;
    ObservableImpl zip(std::initializer_list<ObservableImpl> others, const any& transform) const;

    // Scheduling
    ObservableImpl observeOn(const SchedulerImpl& scheduler) const;

    // Misc
    juce::Array<any> toArray(const std::function<void(std::exception_ptr)>& onError) const;

    // Default error/completion handlers
    static void TerminateOnError(std::exception_ptr);
    static void EmptyOnCompleted();
    
    // The maximum number of parameters for operators like combineLatest, merge, zip, etc., NOT including the observable it is called on
    static const int MaximumArity = 7;

    // The wrapped rxcpp::observable<any>
    any wrapped;
};
}
