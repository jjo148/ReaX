namespace {
std::chrono::milliseconds durationFromRelativeTime(const juce::RelativeTime& relativeTime)
{
    return std::chrono::milliseconds(relativeTime.inMilliseconds());
}

const std::runtime_error InvalidRangeError("Invalid range.");
using detail::any;

// An Observable that holds a Value to keep receiving changes until the Observable is destroyed.
class ValueObservable : private Value::Listener
{
public:
    ValueObservable(const Value& inputValue)
    : value(inputValue),
      subject(inputValue)
    {
        value.addListener(this);
    }

    ~ValueObservable()
    {
        value.removeListener(this);
        subject.get_subscriber().on_completed();
    }

    void valueChanged(Value& newValue) override
    {
        subject.get_subscriber().on_next(newValue);
    }

    rxcpp::observable<var> getObservable() const
    {
        return subject.get_observable();
    }

private:
    Value value;
    const rxcpp::subjects::behavior<var> subject;
};

using Function2 = std::function<any(const any&, const any&)>;
using Function3 = std::function<any(const any&, const any&, const any&)>;
using Function4 = std::function<any(const any&, const any&, const any&, const any&)>;
using Function5 = std::function<any(const any&, const any&, const any&, const any&, const any&)>;
using Function6 = std::function<any(const any&, const any&, const any&, const any&, const any&, const any&)>;
using Function7 = std::function<any(const any&, const any&, const any&, const any&, const any&, const any&, const any&)>;
using Function8 = std::function<any(const any&, const any&, const any&, const any&, const any&, const any&, const any&, const any&)>;
using Function9 = std::function<any(const any&, const any&, const any&, const any&, const any&, const any&, const any&, const any&, const any&)>;

inline any wrap(const rxcpp::observable<any>& observable)
{
    return any(observable);
}

inline rxcpp::observable<any> unwrap(const any& wrapped)
{
    if (wrapped.is<rxcpp::observable<any>>())
        return wrapped.get<rxcpp::observable<any>>();
    else
        return wrapped.get<std::shared_ptr<ValueObservable>>()->getObservable().map([](const var& item) { return any(item); });
}

template<typename T>
rxcpp::observable<any> _range(const T& first, const T& last, unsigned int step)
{
    if (first > last)
        throw InvalidRangeError;

    auto o = rxcpp::observable<>::range<T>(first, last, step, rxcpp::identity_immediate());

    return o.map([](const T& item) { return any(item); });
}

template<typename Transform, typename... Os>
rxcpp::observable<any> _combineLatest(const any& wrapped, Transform&& transform, Os&&... observables)
{
    return unwrap(wrapped).combine_latest(transform, unwrap(observables.wrapped)...);
}

template<typename... Os>
rxcpp::observable<any> _concat(const any& wrapped, Os&&... observables)
{
    return unwrap(wrapped).concat(unwrap(observables.wrapped)...);
}

template<typename... Os>
rxcpp::observable<any> _merge(const any& wrapped, Os&&... observables)
{
    return unwrap(wrapped).merge(unwrap(observables.wrapped)...);
}

template<typename... Items>
rxcpp::observable<any> _startWith(const any& wrapped, Items&&... items)
{
    return unwrap(wrapped).start_with(items...);
}

template<typename Transform, typename... Os>
rxcpp::observable<any> _withLatestFrom(const any& wrapped, Transform&& transform, Os&&... observables)
{
    return unwrap(wrapped).with_latest_from(transform, unwrap(observables.wrapped)...);
}

template<typename Transform, typename... Os>
rxcpp::observable<any> _zip(const any& wrapped, Transform&& transform, Os&&... observables)
{
    return unwrap(wrapped).zip(transform, unwrap(observables.wrapped)...);
}
}

namespace detail {

#pragma mark - Creation

ObservableImpl::ObservableImpl(const any& wrapped)
: wrapped(wrapped)
{}

ObservableImpl ObservableImpl::create(const std::function<void(ObserverImpl&&)>& onSubscribe)
{
    return wrap(rxcpp::observable<>::create<any>([onSubscribe](const rxcpp::subscriber<any>& s) {
        onSubscribe(ObserverImpl(any(s)));
    }));
}

ObservableImpl ObservableImpl::defer(const std::function<ObservableImpl()>& factory)
{
    return wrap(rxcpp::observable<>::defer([factory]() {
        return factory().wrapped.get<rxcpp::observable<any>>();
    }));
}

ObservableImpl ObservableImpl::empty()
{
    return from({});
}

ObservableImpl ObservableImpl::error(const std::exception& error)
{
    return wrap(rxcpp::observable<>::error<any>(error));
}

ObservableImpl ObservableImpl::from(Array<any>&& items)
{
    return wrap(rxcpp::observable<>::iterate(std::move(items), rxcpp::identity_immediate()));
}

ObservableImpl ObservableImpl::fromValue(Value value)
{
    return any(std::make_shared<ValueObservable>(value));
}

ObservableImpl ObservableImpl::interval(const juce::RelativeTime& period)
{
    auto o = rxcpp::observable<>::interval(durationFromRelativeTime(period));
    return wrap(o.map([](long long item) { return any(item); }));
}

ObservableImpl ObservableImpl::just(const any& value)
{
    return wrap(rxcpp::observable<>::just(value));
}

ObservableImpl ObservableImpl::never()
{
    return wrap(rxcpp::observable<>::never<any>());
}

ObservableImpl ObservableImpl::integralRange(long long first, long long last, unsigned int step)
{
    return wrap(_range(first, last, step));
}

ObservableImpl ObservableImpl::floatRange(float first, float last, unsigned int step)
{
    return wrap(_range(first, last, step));
}

ObservableImpl ObservableImpl::doubleRange(double first, double last, unsigned int step)
{
    return wrap(_range(first, last, step));
}

ObservableImpl ObservableImpl::repeat(const any& item)
{
    return wrap(rxcpp::observable<>::just(item).repeat());
}

ObservableImpl ObservableImpl::repeat(const any& item, unsigned int times)
{
    return wrap(rxcpp::observable<>::just(item).repeat(times));
}


#pragma mark - Disposable

Disposable ObservableImpl::subscribe(const std::function<void(const any&)>& onNext,
                                     const std::function<void(std::exception_ptr)>& onError,
                                     const std::function<void()>& onCompleted) const
{
    auto disposable = unwrap(wrapped).subscribe(onNext, onError, onCompleted);

    return Disposable(any(disposable));
}

Disposable ObservableImpl::subscribe(const ObserverImpl& observer) const
{
    auto subscriber = observer.wrapped.get<rxcpp::subscriber<any>>();
    auto disposable = unwrap(wrapped).subscribe(subscriber);

    return Disposable(any(disposable));
}


#pragma mark - Operators

#define VARX_OBSERVABLE_IMPL_UNROLLED_LIST_IMPLEMENTATION(__functionName, __list) \
    const auto it = __list.begin(); \
    switch (__list.size()) { \
        case 1: \
            return wrap(JUCE_JOIN_MACRO(_, __functionName)(wrapped, *it)); \
        case 2: \
            return wrap(JUCE_JOIN_MACRO(_, __functionName)(wrapped, *it, *(it + 1))); \
        case 3: \
            return wrap(JUCE_JOIN_MACRO(_, __functionName)(wrapped, *it, *(it + 1), *(it + 2))); \
        case 4: \
            return wrap(JUCE_JOIN_MACRO(_, __functionName)(wrapped, *it, *(it + 1), *(it + 2), *(it + 3))); \
        case 5: \
            return wrap(JUCE_JOIN_MACRO(_, __functionName)(wrapped, *it, *(it + 1), *(it + 2), *(it + 3), *(it + 4))); \
        case 6: \
            return wrap(JUCE_JOIN_MACRO(_, __functionName)(wrapped, *it, *(it + 1), *(it + 2), *(it + 3), *(it + 4), *(it + 5))); \
        case 7: \
            return wrap(JUCE_JOIN_MACRO(_, __functionName)(wrapped, *it, *(it + 1), *(it + 2), *(it + 3), *(it + 4), *(it + 5), *(it + 6))); \
        case 8: \
            return wrap(JUCE_JOIN_MACRO(_, __functionName)(wrapped, *it, *(it + 1), *(it + 2), *(it + 3), *(it + 4), *(it + 5), *(it + 6), *(it + 7))); \
        default: \
            jassertfalse; \
    }

#define VARX_OBSERVABLE_IMPL_UNROLLED_LIST_IMPLEMENTATION_WITH_TRANSFORM(__functionName, __list, __transform) \
    const auto it = __list.begin(); \
    switch (__list.size()) { \
        case 1: \
            return wrap(JUCE_JOIN_MACRO(_, __functionName)(wrapped, __transform.get<Function2>(), *it)); \
        case 2: \
            return wrap(JUCE_JOIN_MACRO(_, __functionName)(wrapped, __transform.get<Function3>(), *it, *(it + 1))); \
        case 3: \
            return wrap(JUCE_JOIN_MACRO(_, __functionName)(wrapped, __transform.get<Function4>(), *it, *(it + 1), *(it + 2))); \
        case 4: \
            return wrap(JUCE_JOIN_MACRO(_, __functionName)(wrapped, __transform.get<Function5>(), *it, *(it + 1), *(it + 2), *(it + 3))); \
        case 5: \
            return wrap(JUCE_JOIN_MACRO(_, __functionName)(wrapped, __transform.get<Function6>(), *it, *(it + 1), *(it + 2), *(it + 3), *(it + 4))); \
        case 6: \
            return wrap(JUCE_JOIN_MACRO(_, __functionName)(wrapped, __transform.get<Function7>(), *it, *(it + 1), *(it + 2), *(it + 3), *(it + 4), *(it + 5))); \
        case 7: \
            return wrap(JUCE_JOIN_MACRO(_, __functionName)(wrapped, __transform.get<Function8>(), *it, *(it + 1), *(it + 2), *(it + 3), *(it + 4), *(it + 5), *(it + 6))); \
        case 8: \
            return wrap(JUCE_JOIN_MACRO(_, __functionName)(wrapped, __transform.get<Function9>(), *it, *(it + 1), *(it + 2), *(it + 3), *(it + 4), *(it + 5), *(it + 6), *(it + 7))); \
        default: \
            jassertfalse; \
    }

ObservableImpl ObservableImpl::combineLatest(std::initializer_list<ObservableImpl> others, const any& transform) const {
    VARX_OBSERVABLE_IMPL_UNROLLED_LIST_IMPLEMENTATION_WITH_TRANSFORM(combineLatest, others, transform)
}

ObservableImpl ObservableImpl::concat(const Array<ObservableImpl>& others) const
{
    VARX_OBSERVABLE_IMPL_UNROLLED_LIST_IMPLEMENTATION(concat, others);
}

ObservableImpl ObservableImpl::debounce(const juce::RelativeTime& period) const
{
    return wrap(unwrap(wrapped).debounce(durationFromRelativeTime(period)));
}

ObservableImpl ObservableImpl::distinctUntilChanged(const std::function<bool(const any&, const any&)>& equals) const
{
    return wrap(unwrap(wrapped).distinct_until_changed(equals));
}

ObservableImpl ObservableImpl::elementAt(int index) const
{
    return wrap(unwrap(wrapped).element_at(index));
}

ObservableImpl ObservableImpl::filter(const std::function<bool(const any&)>& predicate) const
{
    return wrap(unwrap(wrapped).filter(predicate));
}

ObservableImpl ObservableImpl::flatMap(const std::function<ObservableImpl(const any&)>& f) const
{
    return wrap(unwrap(wrapped).flat_map([f](const any& value) {
        return unwrap(f(value).wrapped);
    }));
}

ObservableImpl ObservableImpl::map(const std::function<any(const any&)>& transform) const
{
    return wrap(unwrap(wrapped).map(transform));
}

#warning Add a test for arity 8
ObservableImpl ObservableImpl::merge(const juce::Array<ObservableImpl>& others) const {
    VARX_OBSERVABLE_IMPL_UNROLLED_LIST_IMPLEMENTATION(merge, others)
}

ObservableImpl ObservableImpl::reduce(const any& startValue, const std::function<any(const any&, const any&)>& f) const
{
    return wrap(unwrap(wrapped).reduce(startValue, f));
}

ObservableImpl ObservableImpl::sample(const juce::RelativeTime& interval) const
{
    return wrap(unwrap(wrapped).sample_with_time(durationFromRelativeTime(interval)));
}

ObservableImpl ObservableImpl::scan(const any& startValue, const std::function<any(const any&, const any&)>& f) const
{
    return wrap(unwrap(wrapped).scan(startValue, f));
}

ObservableImpl ObservableImpl::skip(unsigned int numItems) const
{
    return wrap(unwrap(wrapped).skip(numItems));
}

ObservableImpl ObservableImpl::skipUntil(const ObservableImpl& other) const
{
    return wrap(unwrap(wrapped).skip_until(unwrap(other.wrapped)));
}

ObservableImpl ObservableImpl::startWith(juce::Array<any>&& items) const
{
    VARX_OBSERVABLE_IMPL_UNROLLED_LIST_IMPLEMENTATION(startWith, items);
}

ObservableImpl ObservableImpl::switchOnNext() const
{
    rxcpp::observable<rxcpp::observable<any>> unwrapped = unwrap(wrapped).map([](const any& observable) {
        return unwrap(observable.get<ObservableImpl>().wrapped);
    });

    return wrap(unwrapped.switch_on_next());
}

ObservableImpl ObservableImpl::take(unsigned int numItems) const
{
    return wrap(unwrap(wrapped).take(numItems));
}

ObservableImpl ObservableImpl::takeLast(unsigned int numItems) const
{
    return wrap(unwrap(wrapped).take_last(numItems));
}

ObservableImpl ObservableImpl::takeUntil(const ObservableImpl& other) const
{
    return wrap(unwrap(wrapped).take_until(unwrap(other.wrapped)));
}

ObservableImpl ObservableImpl::takeWhile(const std::function<bool(const any&)>& predicate) const
{
    return wrap(unwrap(wrapped).take_while(predicate));
}

ObservableImpl ObservableImpl::withLatestFrom(std::initializer_list<ObservableImpl> others, const any& transform) const {
    VARX_OBSERVABLE_IMPL_UNROLLED_LIST_IMPLEMENTATION_WITH_TRANSFORM(withLatestFrom, others, transform)
}

ObservableImpl ObservableImpl::zip(std::initializer_list<ObservableImpl> others, const any& transform) const
{
    VARX_OBSERVABLE_IMPL_UNROLLED_LIST_IMPLEMENTATION_WITH_TRANSFORM(zip, others, transform)
}


#pragma mark - Scheduling

ObservableImpl ObservableImpl::observeOn(const SchedulerImpl& scheduler) const
{
    return wrap(scheduler.schedule(unwrap(wrapped)));
}


#pragma mark - Misc

juce::Array<any> ObservableImpl::toArray(const std::function<void(std::exception_ptr)>& onError) const
{
    Array<any> items;

    unwrap(wrapped).as_blocking().subscribe([&](const any& item) {
        items.add(item);
    },
                                            onError);

    return items;
}

void ObservableImpl::TerminateOnError(std::exception_ptr)
{
    // error implicitly ignored, abort
    std::terminate();
}

void ObservableImpl::EmptyOnCompleted()
{}
}
