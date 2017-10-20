#pragma once

class Scheduler;

namespace detail {
struct ObserverImpl;
struct SchedulerImpl;

struct ObservableImpl
{
    ObservableImpl(const any& wrapped);

    // Helper typedefs
    using Function2 = std::function<any(const any&, const any&)>;
    using Function3 = std::function<any(const any&, const any&, const any&)>;
    using Function4 = std::function<any(const any&, const any&, const any&, const any&)>;
    using Function5 = std::function<any(const any&, const any&, const any&, const any&, const any&)>;
    using Function6 = std::function<any(const any&, const any&, const any&, const any&, const any&, const any&)>;
    using Function7 = std::function<any(const any&, const any&, const any&, const any&, const any&, const any&, const any&)>;
    using Function8 = std::function<any(const any&, const any&, const any&, const any&, const any&, const any&, const any&, const any&)>;

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
    Disposable subscribe(const std::function<void(const any&)>& onNext,
                         const std::function<void(std::exception_ptr)>& onError,
                         const std::function<void()>& onCompleted) const;
    Disposable subscribe(const ObserverImpl& observer) const;

    // Operators
    ObservableImpl combineLatest(const ObservableImpl& o1, const Function2& transform) const;
    ObservableImpl combineLatest(const ObservableImpl& o1, const ObservableImpl& o2, const Function3& transform) const;
    ObservableImpl combineLatest(const ObservableImpl& o1, const ObservableImpl& o2, const ObservableImpl& o3, const Function4& transform) const;
    ObservableImpl combineLatest(const ObservableImpl& o1, const ObservableImpl& o2, const ObservableImpl& o3, const ObservableImpl& o4, const Function5& transform) const;
    ObservableImpl combineLatest(const ObservableImpl& o1, const ObservableImpl& o2, const ObservableImpl& o3, const ObservableImpl& o4, const ObservableImpl& o5, const Function6& transform) const;
    ObservableImpl combineLatest(const ObservableImpl& o1, const ObservableImpl& o2, const ObservableImpl& o3, const ObservableImpl& o4, const ObservableImpl& o5, const ObservableImpl& o6, const Function7& transform) const;
    ObservableImpl combineLatest(const ObservableImpl& o1, const ObservableImpl& o2, const ObservableImpl& o3, const ObservableImpl& o4, const ObservableImpl& o5, const ObservableImpl& o6, const ObservableImpl& o7, const Function8& transform) const;
    ObservableImpl concat(const ObservableImpl& o1) const;
    ObservableImpl debounce(const juce::RelativeTime& interval) const;
    ObservableImpl distinctUntilChanged(const std::function<bool(const any&, const any&)>& equals) const;
    ObservableImpl elementAt(int index) const;
    ObservableImpl filter(const std::function<bool(const any&)>& predicate) const;
    ObservableImpl flatMap(const std::function<ObservableImpl(const any&)>& transform) const;
    ObservableImpl map(const std::function<any(const any&)>& transform) const;
    ObservableImpl merge(const ObservableImpl& o1) const;
    ObservableImpl reduce(const any& startValue, const Function2& f) const;
    ObservableImpl sample(const juce::RelativeTime& interval) const;
    ObservableImpl scan(const any& startValue, const Function2& f) const;
    ObservableImpl skip(unsigned int numItems) const;
    ObservableImpl skipUntil(const ObservableImpl& other) const;
    ObservableImpl startWith(const any& v1) const;
    ObservableImpl startWith(const any& v1, const any& v2) const;
    ObservableImpl startWith(const any& v1, const any& v2, const any& v3) const;
    ObservableImpl startWith(const any& v1, const any& v2, const any& v3, const any& v4) const;
    ObservableImpl startWith(const any& v1, const any& v2, const any& v3, const any& v4, const any& v5) const;
    ObservableImpl startWith(const any& v1, const any& v2, const any& v3, const any& v4, const any& v5, const any& v6) const;
    ObservableImpl startWith(const any& v1, const any& v2, const any& v3, const any& v4, const any& v5, const any& v6, const any& v7) const;
    ObservableImpl switchOnNext() const;
    ObservableImpl take(unsigned int numItems) const;
    ObservableImpl takeLast(unsigned int numItems) const;
    ObservableImpl takeUntil(const ObservableImpl& other) const;
    ObservableImpl takeWhile(const std::function<bool(const any&)>& predicate) const;
    ObservableImpl withLatestFrom(const ObservableImpl& o1, const Function2& transform) const;
    ObservableImpl withLatestFrom(const ObservableImpl& o1, const ObservableImpl& o2, const Function3& transform) const;
    ObservableImpl withLatestFrom(const ObservableImpl& o1, const ObservableImpl& o2, const ObservableImpl& o3, const Function4& transform) const;
    ObservableImpl withLatestFrom(const ObservableImpl& o1, const ObservableImpl& o2, const ObservableImpl& o3, const ObservableImpl& o4, const Function5& transform) const;
    ObservableImpl withLatestFrom(const ObservableImpl& o1, const ObservableImpl& o2, const ObservableImpl& o3, const ObservableImpl& o4, const ObservableImpl& o5, const Function6& transform) const;
    ObservableImpl withLatestFrom(const ObservableImpl& o1, const ObservableImpl& o2, const ObservableImpl& o3, const ObservableImpl& o4, const ObservableImpl& o5, const ObservableImpl& o6, const Function7& transform) const;
    ObservableImpl withLatestFrom(const ObservableImpl& o1, const ObservableImpl& o2, const ObservableImpl& o3, const ObservableImpl& o4, const ObservableImpl& o5, const ObservableImpl& o6, const ObservableImpl& o7, const Function8& transform) const;
    ObservableImpl zip(const ObservableImpl& o1, const Function2& transform) const;
    ObservableImpl zip(const ObservableImpl& o1, const ObservableImpl& o2, const Function3& transform) const;
    ObservableImpl zip(const ObservableImpl& o1, const ObservableImpl& o2, const ObservableImpl& o3, const Function4& transform) const;
    ObservableImpl zip(const ObservableImpl& o1, const ObservableImpl& o2, const ObservableImpl& o3, const ObservableImpl& o4, const Function5& transform) const;
    ObservableImpl zip(const ObservableImpl& o1, const ObservableImpl& o2, const ObservableImpl& o3, const ObservableImpl& o4, const ObservableImpl& o5, const Function6& transform) const;
    ObservableImpl zip(const ObservableImpl& o1, const ObservableImpl& o2, const ObservableImpl& o3, const ObservableImpl& o4, const ObservableImpl& o5, const ObservableImpl& o6, const Function7& transform) const;
    ObservableImpl zip(const ObservableImpl& o1, const ObservableImpl& o2, const ObservableImpl& o3, const ObservableImpl& o4, const ObservableImpl& o5, const ObservableImpl& o6, const ObservableImpl& o7, const Function8& transform) const;

    // Scheduling
    ObservableImpl observeOn(const SchedulerImpl& scheduler) const;

    // Misc
    juce::Array<any> toArray(const std::function<void(std::exception_ptr)>& onError) const;

    static void TerminateOnError(std::exception_ptr);
    static void EmptyOnCompleted();

    any wrapped;
};
}
