namespace {
    std::chrono::milliseconds durationFromRelativeTime(const juce::RelativeTime& relativeTime)
    {
        return std::chrono::milliseconds(relativeTime.inMilliseconds());
    }
}

#pragma mark - Creation

ObservableBase::ObservableBase(const Impl_ptr& impl)
: impl(impl)
{}

ObservableBase::Impl_ptr ObservableBase::create(const std::function<void(detail::ObserverImpl&&)>& onSubscribe)
{
    return Impl::fromRxCpp(rxcpp::observable<>::create<var>([onSubscribe](const rxcpp_subscriber& s) {
        onSubscribe(detail::ObserverImpl(s));
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

Disposable ObservableBase::subscribe(const detail::ObserverImpl& observer) const
{
    auto disposable = impl->wrapped.subscribe(observer.wrapped.get<rxcpp_subscriber>());
    
    return Disposable(std::make_shared<Disposable::Impl>(disposable));
}


#pragma mark - Operators

ObservableBase::Impl_ptr ObservableBase::combineLatest(const ObservableBase& o1, const Function2& transform) const
{
    return impl->combineLatest(transform, o1);
}

ObservableBase::Impl_ptr ObservableBase::combineLatest(const ObservableBase& o1, const ObservableBase& o2, const Function3& transform) const
{
    return impl->combineLatest(transform, o1, o2);
}

ObservableBase::Impl_ptr ObservableBase::combineLatest(const ObservableBase& o1, const ObservableBase& o2, const ObservableBase& o3, const Function4& transform) const
{
    return impl->combineLatest(transform, o1, o2, o3);
}

ObservableBase::Impl_ptr ObservableBase::combineLatest(const ObservableBase& o1, const ObservableBase& o2, const ObservableBase& o3, const ObservableBase& o4, const Function5& transform) const
{
    return impl->combineLatest(transform, o1, o2, o3, o4);
}

ObservableBase::Impl_ptr ObservableBase::combineLatest(const ObservableBase& o1, const ObservableBase& o2, const ObservableBase& o3, const ObservableBase& o4, const ObservableBase& o5, const Function6& transform) const
{
    return impl->combineLatest(transform, o1, o2, o3, o4, o5);
}

ObservableBase::Impl_ptr ObservableBase::combineLatest(const ObservableBase& o1, const ObservableBase& o2, const ObservableBase& o3, const ObservableBase& o4, const ObservableBase& o5, const ObservableBase& o6, const Function7& transform) const
{
    return impl->combineLatest(transform, o1, o2, o3, o4, o5, o6);
}

ObservableBase::Impl_ptr ObservableBase::combineLatest(const ObservableBase& o1, const ObservableBase& o2, const ObservableBase& o3, const ObservableBase& o4, const ObservableBase& o5, const ObservableBase& o6, const ObservableBase& o7, const Function8& transform) const
{
    return impl->combineLatest(transform, o1, o2, o3, o4, o5, o6, o7);
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


ObservableBase::Impl_ptr ObservableBase::startWith(const var& v1) const
{
    return impl->startWith(v1);
}

ObservableBase::Impl_ptr ObservableBase::startWith(const var& v1, const var& v2) const
{
    return impl->startWith(v1, v2);
}

ObservableBase::Impl_ptr ObservableBase::startWith(const var& v1, const var& v2, const var& v3) const
{
    return impl->startWith(v1, v2, v3);
}

ObservableBase::Impl_ptr ObservableBase::startWith(const var& v1, const var& v2, const var& v3, const var& v4) const
{
    return impl->startWith(v1, v2, v3, v4);
}

ObservableBase::Impl_ptr ObservableBase::startWith(const var& v1, const var& v2, const var& v3, const var& v4, const var& v5) const
{
    return impl->startWith(v1, v2, v3, v4, v5);
}

ObservableBase::Impl_ptr ObservableBase::startWith(const var& v1, const var& v2, const var& v3, const var& v4, const var& v5, const var& v6) const
{
    return impl->startWith(v1, v2, v3, v4, v5, v6);
}

ObservableBase::Impl_ptr ObservableBase::startWith(const var& v1, const var& v2, const var& v3, const var& v4, const var& v5, const var& v6, const var& v7) const
{
    return impl->startWith(v1, v2, v3, v4, v5, v6, v7);
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

ObservableBase::Impl_ptr ObservableBase::withLatestFrom(const ObservableBase& o1, const Function2& transform) const
{
    return impl->withLatestFrom(transform, o1);
}

ObservableBase::Impl_ptr ObservableBase::withLatestFrom(const ObservableBase& o1, const ObservableBase& o2, const Function3& transform) const
{
    return impl->withLatestFrom(transform, o1, o2);
}

ObservableBase::Impl_ptr ObservableBase::withLatestFrom(const ObservableBase& o1, const ObservableBase& o2, const ObservableBase& o3, const Function4& transform) const
{
    return impl->withLatestFrom(transform, o1, o2, o3);
}

ObservableBase::Impl_ptr ObservableBase::withLatestFrom(const ObservableBase& o1, const ObservableBase& o2, const ObservableBase& o3, const ObservableBase& o4, const Function5& transform) const
{
    return impl->withLatestFrom(transform, o1, o2, o3, o4);
}

ObservableBase::Impl_ptr ObservableBase::withLatestFrom(const ObservableBase& o1, const ObservableBase& o2, const ObservableBase& o3, const ObservableBase& o4, const ObservableBase& o5, const Function6& transform) const
{
    return impl->withLatestFrom(transform, o1, o2, o3, o4, o5);
}

ObservableBase::Impl_ptr ObservableBase::withLatestFrom(const ObservableBase& o1, const ObservableBase& o2, const ObservableBase& o3, const ObservableBase& o4, const ObservableBase& o5, const ObservableBase& o6, const Function7& transform) const
{
    return impl->withLatestFrom(transform, o1, o2, o3, o4, o5, o6);
}

ObservableBase::Impl_ptr ObservableBase::withLatestFrom(const ObservableBase& o1, const ObservableBase& o2, const ObservableBase& o3, const ObservableBase& o4, const ObservableBase& o5, const ObservableBase& o6, const ObservableBase& o7, const Function8& transform) const
{
    return impl->withLatestFrom(transform, o1, o2, o3, o4, o5, o6, o7);
}

ObservableBase::Impl_ptr ObservableBase::zip(const ObservableBase& o1, const Function2& transform) const
{
    return impl->zip(transform, o1);
}

ObservableBase::Impl_ptr ObservableBase::zip(const ObservableBase& o1, const ObservableBase& o2, const Function3& transform) const
{
    return impl->zip(transform, o1, o2);
}

ObservableBase::Impl_ptr ObservableBase::zip(const ObservableBase& o1, const ObservableBase& o2, const ObservableBase& o3, const Function4& transform) const
{
    return impl->zip(transform, o1, o2, o3);
}

ObservableBase::Impl_ptr ObservableBase::zip(const ObservableBase& o1, const ObservableBase& o2, const ObservableBase& o3, const ObservableBase& o4, const Function5& transform) const
{
    return impl->zip(transform, o1, o2, o3, o4);
}

ObservableBase::Impl_ptr ObservableBase::zip(const ObservableBase& o1, const ObservableBase& o2, const ObservableBase& o3, const ObservableBase& o4, const ObservableBase& o5, const Function6& transform) const
{
    return impl->zip(transform, o1, o2, o3, o4, o5);
}

ObservableBase::Impl_ptr ObservableBase::zip(const ObservableBase& o1, const ObservableBase& o2, const ObservableBase& o3, const ObservableBase& o4, const ObservableBase& o5, const ObservableBase& o6, const Function7& transform) const
{
    return impl->zip(transform, o1, o2, o3, o4, o5, o6);
}

ObservableBase::Impl_ptr ObservableBase::zip(const ObservableBase& o1, const ObservableBase& o2, const ObservableBase& o3, const ObservableBase& o4, const ObservableBase& o5, const ObservableBase& o6, const ObservableBase& o7, const Function8& transform) const
{
    return impl->zip(transform, o1, o2, o3, o4, o5, o6, o7);
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
