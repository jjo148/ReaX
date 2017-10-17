namespace {
    const std::runtime_error InvalidRangeError("Invalid range.");

    std::chrono::milliseconds durationFromRelativeTime(const juce::RelativeTime& relativeTime)
    {
        return std::chrono::milliseconds(relativeTime.inMilliseconds());
    }
}


const std::function<void(Error)> Observable::TerminateOnError = [](Error) {
    // error implicitly ignored, abort
    std::terminate();
};

const std::function<void()> Observable::EmptyOnCompleted = []() {};


#pragma mark - Creation

Observable::Observable(const shared_ptr<Impl>& impl)
: impl(impl)
{}

Observable Observable::create(const std::function<void(Observer)>& onSubscribe)
{
    return Impl::fromRxCpp(rxcpp::observable<>::create<var>([onSubscribe](rxcpp::subscriber<var> s) {
        onSubscribe(Observer(std::make_shared<Observer::Impl>(s)));
    }));
}

Observable Observable::defer(const std::function<Observable()>& factory)
{
    return Impl::fromRxCpp(rxcpp::observable<>::defer([factory]() {
        return factory().impl->wrapped;
    }));
}

Observable Observable::empty()
{
    return Impl::fromRxCpp(rxcpp::observable<>::empty<var>());
}

Observable Observable::error(const std::exception& error)
{
    return Impl::fromRxCpp(rxcpp::observable<>::error<var>(error));
}

Observable Observable::from(const Array<var>& array)
{
    return Impl::fromRxCpp(rxcpp::observable<>::iterate(array, rxcpp::identity_immediate()));
}

Observable Observable::fromValue(Value value)
{
    return Impl::fromValue(value);
}

Observable Observable::interval(const juce::RelativeTime& period)
{
    auto o = rxcpp::observable<>::interval(durationFromRelativeTime(period));
    return Impl::fromRxCpp(o.map(toVar<int>));
}

std::shared_ptr<Observable::Impl> Observable::_just(const var& value)
{
    return Impl::fromRxCpp(rxcpp::observable<>::just(value));
}

Observable Observable::never()
{
    return Impl::fromRxCpp(rxcpp::observable<>::never<var>());
}

Observable Observable::range(int first, int last, unsigned int step)
{
    if (first > last)
        throw InvalidRangeError;

    auto o = rxcpp::observable<>::range<int>(first, last, step, rxcpp::identity_immediate());

    return Impl::fromRxCpp(o.map([](int i) { return toVar(i); }));
}

Observable Observable::range(double first, double last, unsigned int step)
{
    if (first > last)
        throw InvalidRangeError;

    auto o = rxcpp::observable<>::range<double>(first, last, step, rxcpp::identity_immediate());

    return Impl::fromRxCpp(o.map([](double d) { return toVar(d); }));
}

Observable Observable::_repeat(const var& item)
{
    return Impl::fromRxCpp(rxcpp::observable<>::just(item).repeat());
}

Observable Observable::_repeat(const var& item, unsigned int times)
{
    return Impl::fromRxCpp(rxcpp::observable<>::just(item).repeat(times));
}


#pragma mark - Disposable

Disposable Observable::subscribe(const std::function<void(const var&)>& onNext,
                                 const std::function<void(Error)>& onError,
                                 const std::function<void()>& onCompleted) const
{
    auto disposable = impl->wrapped.subscribe(onNext, onError, onCompleted);

    return Disposable(std::make_shared<Disposable::Impl>(disposable));
}

Disposable Observable::subscribe(const Observer& observer) const
{
    auto disposable = impl->wrapped.subscribe(observer.impl->wrapped);

    return Disposable(std::make_shared<Disposable::Impl>(disposable));
}


#pragma mark - Operators

Observable Observable::combineLatest(Observable o1, Function2& f) const
{
    return impl->combineLatest(f, o1);
}
Observable Observable::combineLatest(Observable o1, Observable o2, Function3 f) const
{
    return impl->combineLatest(f, o1, o2);
}
Observable Observable::combineLatest(Observable o1, Observable o2, Observable o3, Function4 f) const
{
    return impl->combineLatest(f, o1, o2, o3);
}
Observable Observable::combineLatest(Observable o1, Observable o2, Observable o3, Observable o4, Function5 f) const
{
    return impl->combineLatest(f, o1, o2, o3, o4);
}
Observable Observable::combineLatest(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5, Function6 f) const
{
    return impl->combineLatest(f, o1, o2, o3, o4, o5);
}
Observable Observable::combineLatest(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5, Observable o6, Function7 f) const
{
    return impl->combineLatest(f, o1, o2, o3, o4, o5, o6);
}
Observable Observable::combineLatest(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5, Observable o6, Observable o7, Function8 f) const
{
    return impl->combineLatest(f, o1, o2, o3, o4, o5, o6, o7);
}

Observable Observable::concat(Observable o1) const
{
    return impl->concat(o1);
}
Observable Observable::concat(Observable o1, Observable o2) const
{
    return impl->concat(o1, o2);
}
Observable Observable::concat(Observable o1, Observable o2, Observable o3) const
{
    return impl->concat(o1, o2, o3);
}
Observable Observable::concat(Observable o1, Observable o2, Observable o3, Observable o4) const
{
    return impl->concat(o1, o2, o3, o4);
}
Observable Observable::concat(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5) const
{
    return impl->concat(o1, o2, o3, o4, o5);
}
Observable Observable::concat(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5, Observable o6) const
{
    return impl->concat(o1, o2, o3, o4, o5, o6);
}
Observable Observable::concat(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5, Observable o6, Observable o7) const
{
    return impl->concat(o1, o2, o3, o4, o5, o6, o7);
}

Observable Observable::debounce(const juce::RelativeTime& period) const
{
    return Impl::fromRxCpp(impl->wrapped.debounce(durationFromRelativeTime(period)));
}

Observable Observable::_distinctUntilChanged(Predicate2 equals) const
{
    return Impl::fromRxCpp(impl->wrapped.distinct_until_changed(equals));
}

Observable Observable::elementAt(int index) const
{
    return Impl::fromRxCpp(impl->wrapped.element_at(index));
}

Observable Observable::filter(Predicate1 predicate) const
{
    return Impl::fromRxCpp(impl->wrapped.filter(predicate));
}

Observable Observable::flatMap(const std::function<Observable(const var&)>& f) const
{
    return Impl::fromRxCpp(impl->wrapped.flat_map([f](const var& value) {
        return f(value).impl->wrapped;
    }));
}

Observable Observable::map(Function1 f) const
{
    return Impl::fromRxCpp(impl->wrapped.map(f));
}

Observable Observable::merge(Observable o1) const
{
    return impl->merge(o1);
}
Observable Observable::merge(Observable o1, Observable o2) const
{
    return impl->merge(o1, o2);
}
Observable Observable::merge(Observable o1, Observable o2, Observable o3) const
{
    return impl->merge(o1, o2, o3);
}
Observable Observable::merge(Observable o1, Observable o2, Observable o3, Observable o4) const
{
    return impl->merge(o1, o2, o3, o4);
}
Observable Observable::merge(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5) const
{
    return impl->merge(o1, o2, o3, o4, o5);
}
Observable Observable::merge(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5, Observable o6) const
{
    return impl->merge(o1, o2, o3, o4, o5, o6);
}
Observable Observable::merge(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5, Observable o6, Observable o7) const
{
    return impl->merge(o1, o2, o3, o4, o5, o6, o7);
}

Observable Observable::reduce(const var& startValue, Function2 f) const
{
    return Impl::fromRxCpp(impl->wrapped.reduce(startValue, f));
}

Observable Observable::sample(const juce::RelativeTime& interval)
{
    return Impl::fromRxCpp(impl->wrapped.sample_with_time(durationFromRelativeTime(interval)));
}

Observable Observable::scan(const var& startValue, Function2 f) const
{
    return Impl::fromRxCpp(impl->wrapped.scan(startValue, f));
}

Observable Observable::skip(unsigned int numItems) const
{
    return Impl::fromRxCpp(impl->wrapped.skip(numItems));
}

Observable Observable::skipUntil(Observable other) const
{
    return Impl::fromRxCpp(impl->wrapped.skip_until(other.impl->wrapped));
}

Observable Observable::startWith(const var& item1) const
{
    return impl->startWith(item1);
}
Observable Observable::startWith(const var& item1, const var& item2) const
{
    return impl->startWith(item1, item2);
}
Observable Observable::startWith(const var& item1, const var& item2, const var& item3) const
{
    return impl->startWith(item1, item2, item3);
}
Observable Observable::startWith(const var& item1, const var& item2, const var& item3, const var& item4) const
{
    return impl->startWith(item1, item2, item3, item4);
}
Observable Observable::startWith(const var& item1, const var& item2, const var& item3, const var& item4, const var& item5) const
{
    return impl->startWith(item1, item2, item3, item4, item5);
}
Observable Observable::startWith(const var& item1, const var& item2, const var& item3, const var& item4, const var& item5, const var& item6) const
{
    return impl->startWith(item1, item2, item3, item4, item5, item6);
}
Observable Observable::startWith(const var& item1, const var& item2, const var& item3, const var& item4, const var& item5, const var& item6, const var& item7) const
{
    return impl->startWith(item1, item2, item3, item4, item5, item6, item7);
}
Observable Observable::startWith(const var& item1, const var& item2, const var& item3, const var& item4, const var& item5, const var& item6, const var& item7, const var& item8) const
{
    return impl->startWith(item1, item2, item3, item4, item5, item6, item7, item8);
}

Observable Observable::switchOnNext() const
{
    rxcpp::observable<rxcpp::observable<var>> unwrapped = impl->wrapped.map([](var observable) {
        return fromVar<Observable>(observable).impl->wrapped;
    });

    return Impl::fromRxCpp(unwrapped.switch_on_next());
}

Observable Observable::take(unsigned int numItems) const
{
    return Impl::fromRxCpp(impl->wrapped.take(numItems));
}

Observable Observable::takeLast(unsigned int numItems) const
{
    return Impl::fromRxCpp(impl->wrapped.take_last(numItems));
}

Observable Observable::takeUntil(Observable other) const
{
    return Impl::fromRxCpp(impl->wrapped.take_until(other.impl->wrapped));
}

Observable Observable::takeWhile(Predicate1 predicate) const
{
    return Impl::fromRxCpp(impl->wrapped.take_while(predicate));
}

Observable Observable::withLatestFrom(Observable o1, Function2& f) const
{
    return impl->withLatestFrom(f, o1);
}
Observable Observable::withLatestFrom(Observable o1, Observable o2, Function3 f) const
{
    return impl->withLatestFrom(f, o1, o2);
}
Observable Observable::withLatestFrom(Observable o1, Observable o2, Observable o3, Function4 f) const
{
    return impl->withLatestFrom(f, o1, o2, o3);
}
Observable Observable::withLatestFrom(Observable o1, Observable o2, Observable o3, Observable o4, Function5 f) const
{
    return impl->withLatestFrom(f, o1, o2, o3, o4);
}
Observable Observable::withLatestFrom(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5, Function6 f) const
{
    return impl->withLatestFrom(f, o1, o2, o3, o4, o5);
}
Observable Observable::withLatestFrom(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5, Observable o6, Function7 f) const
{
    return impl->withLatestFrom(f, o1, o2, o3, o4, o5, o6);
}
Observable Observable::withLatestFrom(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5, Observable o6, Observable o7, Function8 f) const
{
    return impl->withLatestFrom(f, o1, o2, o3, o4, o5, o6, o7);
}

Observable Observable::zip(Observable o1, Function2& f) const
{
    return impl->zip(f, o1);
}
Observable Observable::zip(Observable o1, Observable o2, Function3 f) const
{
    return impl->zip(f, o1, o2);
}
Observable Observable::zip(Observable o1, Observable o2, Observable o3, Function4 f) const
{
    return impl->zip(f, o1, o2, o3);
}
Observable Observable::zip(Observable o1, Observable o2, Observable o3, Observable o4, Function5 f) const
{
    return impl->zip(f, o1, o2, o3, o4);
}
Observable Observable::zip(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5, Function6 f) const
{
    return impl->zip(f, o1, o2, o3, o4, o5);
}
Observable Observable::zip(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5, Observable o6, Function7 f) const
{
    return impl->zip(f, o1, o2, o3, o4, o5, o6);
}
Observable Observable::zip(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5, Observable o6, Observable o7, Function8 f) const
{
    return impl->zip(f, o1, o2, o3, o4, o5, o6, o7);
}


#pragma mark - Scheduling

Observable Observable::observeOn(const Scheduler& scheduler) const
{
    return Impl::fromRxCpp(scheduler.impl->schedule(impl->wrapped));
}


#pragma mark - Misc

Observable::operator var() const
{
    return toVar(*this);
}

juce::Array<var> Observable::toArray(const std::function<void(Error)>& onError) const
{
    Array<var> items;
    impl->wrapped.as_blocking().subscribe([&](const var& item) {
        items.add(item);
    },
                                          onError);
    return items;
}


#pragma mark - Private

var Observable::CombineIntoArray2(const var& v1, const var& v2)
{
    return Array<var>({ v1, v2 });
}
var Observable::CombineIntoArray3(const var& v1, const var& v2, const var& v3)
{
    return Array<var>({ v1, v2, v3 });
}
var Observable::CombineIntoArray4(const var& v1, const var& v2, const var& v3, const var& v4)
{
    return Array<var>({ v1, v2, v3, v4 });
}
var Observable::CombineIntoArray5(const var& v1, const var& v2, const var& v3, const var& v4, const var& v5)
{
    return Array<var>({ v1, v2, v3, v4, v5 });
}
var Observable::CombineIntoArray6(const var& v1, const var& v2, const var& v3, const var& v4, const var& v5, const var& v6)
{
    return Array<var>({ v1, v2, v3, v4, v5, v6 });
}
var Observable::CombineIntoArray7(const var& v1, const var& v2, const var& v3, const var& v4, const var& v5, const var& v6, const var& v7)
{
    return Array<var>({ v1, v2, v3, v4, v5, v6, v7 });
}
var Observable::CombineIntoArray8(const var& v1, const var& v2, const var& v3, const var& v4, const var& v5, const var& v6, const var& v7, const var& v8)
{
    return Array<var>({ v1, v2, v3, v4, v5, v6, v7, v8 });
}






























#pragma mark - Creation

ObservableBase::ObservableBase(const Impl_ptr& impl)
: impl(impl)
{}

ObservableBase::Impl_ptr ObservableBase::create(const std::function<void(const Observer&)>& onSubscribe)
{
    return Impl::fromRxCpp(rxcpp::observable<>::create<var>([onSubscribe](rxcpp::subscriber<var> s) {
        onSubscribe(Observer(std::make_shared<Observer::Impl>(s)));
    }));
}

ObservableBase::Impl_ptr ObservableBase::defer(const std::function<ObservableBase()>& factory)
{
    return Impl::fromRxCpp(rxcpp::observable<>::defer([factory]() {
        return factory().impl->wrapped;
    }));
}

ObservableBase::Impl_ptr ObservableBase::empty()
{
    return Impl::fromRxCpp(rxcpp::observable<>::empty<var>());
}

ObservableBase::Impl_ptr ObservableBase::error(const std::exception& error)
{
    return Impl::fromRxCpp(rxcpp::observable<>::error<var>(error));
}

ObservableBase::Impl_ptr ObservableBase::from(Array<var>&& items)
{
    return Impl::fromRxCpp(rxcpp::observable<>::iterate(std::move(items), rxcpp::identity_immediate()));
}

ObservableBase::Impl_ptr ObservableBase::fromValue(Value value)
{
    return Impl::fromValue(value);
}

ObservableBase::Impl_ptr ObservableBase::interval(const juce::RelativeTime& period)
{
    auto o = rxcpp::observable<>::interval(durationFromRelativeTime(period));
    return Impl::fromRxCpp(o.map(toVar<long long>));
}

ObservableBase::Impl_ptr ObservableBase::just(const var& value)
{
    return Impl::fromRxCpp(rxcpp::observable<>::just(value));
}

ObservableBase::Impl_ptr ObservableBase::never()
{
    return Impl::fromRxCpp(rxcpp::observable<>::never<var>());
}

ObservableBase::Impl_ptr ObservableBase::integralRange(long long first, long long last, unsigned int step)
{
    return Impl::range(first, last, step);
}

ObservableBase::Impl_ptr ObservableBase::floatRange(float first, float last, unsigned int step)
{
    return Impl::range(first, last, step);
}

ObservableBase::Impl_ptr ObservableBase::doubleRange(double first, double last, unsigned int step)
{
    return Impl::range(first, last, step);
}

ObservableBase::Impl_ptr ObservableBase::repeat(const var& item)
{
    return Impl::fromRxCpp(rxcpp::observable<>::just(item).repeat());
}

ObservableBase::Impl_ptr ObservableBase::repeat(const var& item, unsigned int times)
{
    return Impl::fromRxCpp(rxcpp::observable<>::just(item).repeat(times));
}


#pragma mark - Disposable

Disposable ObservableBase::subscribe(const std::function<void(const var&)>& onNext,
                                     const std::function<void(Error)>& onError,
                                     const std::function<void()>& onCompleted) const
{
    auto disposable = impl->wrapped.subscribe(onNext, onError, onCompleted);
    
    return Disposable(std::make_shared<Disposable::Impl>(disposable));
}

Disposable ObservableBase::subscribe(const Observer& observer) const
{
    auto disposable = impl->wrapped.subscribe(observer.impl->wrapped);
    
    return Disposable(std::make_shared<Disposable::Impl>(disposable));
}


#pragma mark - Operators

ObservableBase::Impl_ptr ObservableBase::combineLatest(const ObservableBase& o1, const std::function<var(const var&, const var&)>& transform) const
{
    return impl->combineLatest(transform, o1);
}

ObservableBase::Impl_ptr ObservableBase::concat(const ObservableBase& o1) const
{
    return impl->concat(o1);
}

ObservableBase::Impl_ptr ObservableBase::debounce(const juce::RelativeTime& period) const
{
    return Impl::fromRxCpp(impl->wrapped.debounce(durationFromRelativeTime(period)));
}

ObservableBase::Impl_ptr ObservableBase::distinctUntilChanged(const std::function<bool(const var&, const var&)>& equals) const
{
    return Impl::fromRxCpp(impl->wrapped.distinct_until_changed(equals));
}

ObservableBase::Impl_ptr ObservableBase::elementAt(int index) const
{
    return Impl::fromRxCpp(impl->wrapped.element_at(index));
}

ObservableBase::Impl_ptr ObservableBase::filter(const std::function<bool(const var&)>& predicate) const
{
    return Impl::fromRxCpp(impl->wrapped.filter(predicate));
}

ObservableBase::Impl_ptr ObservableBase::flatMap(const std::function<ObservableBase(const var&)>& f) const
{
    return Impl::fromRxCpp(impl->wrapped.flat_map([f](const var& value) {
        return f(value).impl->wrapped;
    }));
}

ObservableBase::Impl_ptr ObservableBase::map(const std::function<var(const var&)>& transform) const
{
    return Impl::fromRxCpp(impl->wrapped.map(transform));
}

ObservableBase::Impl_ptr ObservableBase::merge(const ObservableBase& o1) const
{
    return impl->merge(o1);
}

ObservableBase::Impl_ptr ObservableBase::reduce(const var& startValue, const std::function<var(const var&, const var&)>& f) const
{
    return Impl::fromRxCpp(impl->wrapped.reduce(startValue, f));
}

ObservableBase::Impl_ptr ObservableBase::sample(const juce::RelativeTime& interval) const
{
    return Impl::fromRxCpp(impl->wrapped.sample_with_time(durationFromRelativeTime(interval)));
}

ObservableBase::Impl_ptr ObservableBase::scan(const var& startValue, const std::function<var(const var&, const var&)>& f) const
{
    return Impl::fromRxCpp(impl->wrapped.scan(startValue, f));
}

ObservableBase::Impl_ptr ObservableBase::skip(unsigned int numItems) const
{
    return Impl::fromRxCpp(impl->wrapped.skip(numItems));
}

ObservableBase::Impl_ptr ObservableBase::skipUntil(const ObservableBase& other) const
{
    return Impl::fromRxCpp(impl->wrapped.skip_until(other.impl->wrapped));
}

ObservableBase::Impl_ptr ObservableBase::startWith(const var& item1) const
{
    return impl->startWith(item1);
}

ObservableBase::Impl_ptr ObservableBase::switchOnNext() const
{
    rxcpp::observable<rxcpp::observable<var>> unwrapped = impl->wrapped.map([](const var& observable) {
        return fromVar<ObservableBase>(observable).impl->wrapped;
    });
    
    return Impl::fromRxCpp(unwrapped.switch_on_next());
}

ObservableBase::Impl_ptr ObservableBase::take(unsigned int numItems) const
{
    return Impl::fromRxCpp(impl->wrapped.take(numItems));
}

ObservableBase::Impl_ptr ObservableBase::takeLast(unsigned int numItems) const
{
    return Impl::fromRxCpp(impl->wrapped.take_last(numItems));
}

ObservableBase::Impl_ptr ObservableBase::takeUntil(const ObservableBase& other) const
{
    return Impl::fromRxCpp(impl->wrapped.take_until(other.impl->wrapped));
}

ObservableBase::Impl_ptr ObservableBase::takeWhile(const std::function<bool(const var&)>& predicate) const
{
    return Impl::fromRxCpp(impl->wrapped.take_while(predicate));
}

ObservableBase::Impl_ptr ObservableBase::withLatestFrom(const ObservableBase& o1, const std::function<var(const var&, const var&)>& transform) const
{
    return impl->withLatestFrom(transform, o1);
}

ObservableBase::Impl_ptr ObservableBase::zip(const ObservableBase& o1, const std::function<var(const var&, const var&)>& transform) const
{
    return impl->zip(transform, o1);
}


#pragma mark - Scheduling

ObservableBase::Impl_ptr ObservableBase::observeOn(const Scheduler& scheduler) const
{
    return Impl::fromRxCpp(scheduler.impl->schedule(impl->wrapped));
}


#pragma mark - Misc

juce::Array<var> ObservableBase::toArray(const std::function<void(Error)>& onError) const
{
    Array<var> items;
    impl->wrapped.as_blocking().subscribe([&](const var& item) {
        items.add(item);
    }, onError);
    return items;
}

void ObservableBase::TerminateOnError(const Error&)
{
    // error implicitly ignored, abort
    std::terminate();
}

void ObservableBase::EmptyOnCompleted()
{}
