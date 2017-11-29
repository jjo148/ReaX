#pragma once

template<typename T>
class Observer;

/**
 An Observable emits values over time.
 
 Observers can subscribe to Observable​s, to be notified whenever the Observable emits a value.
 
 For an introduction to Observables, please refer to http://reactivex.io/documentation/observable.html.
 */
template<typename T>
class Observable
{
    typedef detail::ObservableImpl Impl;
    typedef detail::any any;
    
public:
#pragma mark - Helpers
    /// The type of values emitted by this Observable.
    typedef T ValueType;

    /// \cond internal
    template<typename U>
    struct IsObservable : std::false_type
    {
    };
    template<typename U>
    struct IsObservable<Observable<U>> : std::true_type
    {
    };
    template<typename Function, typename... Args>
    using CallResult = typename std::result_of<Function(Args...)>::type;
    /// \endcond

#pragma mark - Creation
    /**
     Creates an Observable that doesn't emit any values and notifies onComplete immediately.
     
     This is the same as Observable<T>::empty().
     */
    Observable<T>()
    : impl(Impl::empty())
    {}

    /**
     Creates an Observable which emits values from an Observer on each subscription.
     
     In the onSubscribe function, you get an Observer. You can call Observer::onNext on it to emit values from the Observable.
     */
    static Observable<T> create(const std::function<void(const Observer<T>&)>& onSubscribe)
    {
        return Impl::create([onSubscribe](detail::ObserverImpl&& impl) {
            onSubscribe(Observer<T>(std::move(impl)));
        });
    }

    /**
     Creates a new Observable for each subscriber, by calling the `factory` function on each new subscription.
     */
    static Observable<T> defer(const std::function<Observable<T>()>& factory)
    {
        return Impl::defer([factory]() {
            return factory().impl;
        });
    }

    /**
     Creates an Observable that doesn't emit any values and notifies onComplete immediately.
     */
    static Observable<T> empty()
    {
        return Impl::empty();
    }

    /**
     Creates an Observable which doesn't emit any values, and immediately notifies onError.
     */
    static Observable<T> error(const std::exception& error)
    {
        return Impl::error(error);
    }

    /**
     Creates an Observable that immediately emits the values from the given Array.
     
     Note that you can also pass an initializer list, like this:
     
         Observable<String>::from({"Hello", "Test"})
         Observable<var>::from({var(3), var("four")})
     */
    static Observable<T> from(const juce::Array<T>& array)
    {
        juce::Array<any> values;

        for (auto& value : array)
            values.add(Observable<T>::toAny(value));

        return Impl::from(std::move(values));
    }

    /**
     Creates an Observable from a given JUCE Value. The returned Observable **only emits values until it is destroyed**, so you are responsible for managing its lifetime. Or use Reactive<Value>, which will handle this.
     
     The returned Observable notifies the onComplete handler when it's destroyed. @see Observable::subscribe
     
     When calling Value::setValue, it notifies asynchronously. So **the returned Observable emits the new value asynchronously.** If you call setValue immediately before destroying the returned Observable, the new value will not be emitted.
     */
    template<typename U = T>
    static Observable<T> fromValue(juce::Value value, enable_if_t<std::is_same<U, T>::value && std::is_base_of<juce::var, U>::value>* = 0)
    {
        return Impl::fromValue(value);
    }

    /**
     Returns an Observable that emits one value every `interval`, starting at the time of subscription (where the first value is emitted). The emitted values are `1`, `2`, `3`, and so on.
     
     The Observable emits endlessly, but you can use Observable::take to get a finite number of values (for example).
     
     The interval has millisecond resolution.
     */
    template<typename U = T>
    static Observable<T> interval(const juce::RelativeTime& interval, enable_if_t<std::is_same<U, T>::value && std::is_same<int, T>::value>* = 0)
    {
        return Impl::interval(interval);
    }

    /**
     Creates an Observable which emits a single value.
     
     The value is emitted immediately on each new subscription.
     */
    static Observable<T> just(const T& value)
    {
        return Impl::just(Observable<T>::toAny(value));
    }

    /**
     Creates an Observable that never emits any events and never terminates.
     */
    static Observable<T> never()
    {
        return Impl::never();
    }

    /**
     Creates an Observable which emits a range of values, starting at `first` to (and including) `last`. It completes after emitting the `last` value.
     
     ​ **Throws an exception if first > last.**
     
     For example:
     
         Observable<int>::range(3, 7, 3) // {3, 6, 7}
         Observable<double>::range(17.5, 22.8, 2) // {17.5, 19.5, 21.5, 22.8}
     */
    template<typename U = T>
    static Observable<T> range(T first, T last, unsigned int step = 1, enable_if_t<std::is_same<U, T>::value && std::is_integral<U>::value>* = 0)
    {
        return Impl::integralRange(first, last, step);
    }
    /// \overload
    template<typename U = T>
    static Observable<T> range(float first, float last, unsigned int step = 1, enable_if_t<std::is_same<U, T>::value && std::is_same<U, float>::value>* = 0)
    {
        return Impl::floatRange(first, last, step);
    }
    /// \overload
    template<typename U = T>
    static Observable<T> range(double first, double last, unsigned int step = 1, enable_if_t<std::is_same<U, T>::value && std::is_same<U, double>::value>* = 0)
    {
        return Impl::doubleRange(first, last, step);
    }

    /**
     Creates an Observable which emits a given value repeatedly.
     
     An optional `times` parameter specifies how often the value should be repeated. If omitted, the value will is repeated indefinitely.
     */
    static Observable<T> repeat(const T& value)
    {
        return Impl::repeat(Observable<T>::toAny(value));
    }
    /// \overload
    static Observable<T> repeat(const T& value, unsigned int times)
    {
        return Impl::repeat(Observable<T>::toAny(value), times);
    }


#pragma mark - Subscription
    ///@{
    /**
     Subscribes to an Observable, to receive values it emits.
     
     The **onNext** function is called whenever the Observable emits a new value. It may be called synchronously before `subscribe()` returns.
     
     The **onError** function is called when the Observable has failed to generate the expected data, or has encountered some other error. If `onError` is called, the Observable will not make any more calls. **If you don't pass an onError handler, an exception inside the Observable will terminate your app.**
     
     The **onCompleted** function is called exactly once to notify that the Observable has generated all data and will not emit any more values.
     
     The returned Subscription can be used to `unsubscribe()` from the Observable, to stop receiving values from it. **You will keep receiving values until you call Subscription::unsubscribe, or until the Observable source is destroyed**. The best way is to use a DisposeBag, which automatically unsubscribes when it is destroyed.
     
     Example:
     
         DisposeBag disposeBag;
         Observable<int>::from({4, 17}).subscribe([](int i) {
             std::cout << i << " ";
         }).disposedBy(disposeBag);
     */
    Subscription subscribe(const std::function<void(const T&)>& onNext,
                           const std::function<void(std::exception_ptr)>& onError = Impl::TerminateOnError,
                           const std::function<void()>& onCompleted = Impl::EmptyOnCompleted) const
    {
        return impl.subscribe([onNext](const any& next) {
            onNext(next.get<T>());
        },
                              onError,
                              onCompleted);
    }

    /**
     Subscribes an Observer to an Observable. The Observer is notified whenever the Observable emits a value, or notifies `onError` / `onCompleted`.
     
     The returned Subscription can be used to `unsubscribe()` the Observer, so it stops being notified by this Observable. **The Observer keeps receiving values until you call Subscription::unsubscribe, or until the Observable source is destroyed**. The best way is to use a DisposeBag, which automatically unsubscribes when it is destroyed.
     */
    template<typename U>
    Subscription subscribe(const Observer<U>& observer, enable_if_t<std::is_convertible<T, U>::value>* = 0) const
    {
        // Converts values from T to U
        static auto convert = [](const any& t) {
            return toAny<U>(t.get<T>());
        };

        return impl.map(convert).subscribe(observer.impl);
    }
        ///@}


#pragma mark - Operators
    ///@{
    /**
     Returns an Observable that emits **whenever** a value is emitted by either this Observable **or** one of the `others`. It combines the **latest** value from each Observable via the given function and emits what was returned by the function.
     
     This is different from Observable::withLatestFrom because it emits whenever this Observable or one of the `others` emits a value.
     
     @see Observable::withLatestFrom
     */
    template<typename... Ts>
    Observable<std::tuple<T, Ts...>> combineLatest(const Observable<Ts>&... others) const
    {
        return combineLatest(std::make_tuple<const T&, const Ts&...>, others...);
    }
    /// \overload
    template<typename... Ts, typename Function>
    Observable<CallResult<Function, T, Ts...>> combineLatest(Function&& function, const Observable<Ts>&... others) const
    {
        static_assert(sizeof...(Ts) > 0, "Must pass at least one other Observable to combineLatest.");
        static_assert(sizeof...(Ts) <= Impl::MaximumArity, "Too many Observables passed to combineLatest.");

        const std::function<any(const any&, const typename any_args<Ts>::type&...)> untypedFunction = [function](const any& v, const typename any_args<Ts>::type&... vs) {
            return toAny(function(v.get<T>(), vs.template get<Ts>()...));
        };

        return impl.combineLatest({ others.impl... }, toAny(untypedFunction));
    }
    ///@}

    /**
     Returns an Observable that first emits the values from this Observable, then from the first in the `others` list, then from the second, and so on.
     
     It only subscribes to the first in the `others` list when this Observable has completed. And only subscribes to the second when the first has completed, and so on.
     */
    template<typename... Ts>
    Observable<T> concat(const Observable<Ts>&... others) const
    {
        static_assert(sizeof...(others) > 0, "Must pass at least one other Observable to concat.");
        static_assert(sizeof...(others) <= Impl::MaximumArity, "Too many Observables passed to concat.");
        static_assert(conjunction<std::is_same<T, Ts>...>::value, "All Observables must have the same type.");

        return impl.concat({ others.impl... });
    }

    /**
     Returns an Observable which emits if `interval` has passed without this Observable emitting a value. The returned Observable emits the latest value from this Observable.
     
     For example, think of the instant search in a search engine: Search suggestions are only loaded if the user hasn't pressed a key for a short period of time.
     
     The `interval` has millisecond resolution.
     */
    Observable<T> debounce(const juce::RelativeTime& interval) const
    {
        return impl.debounce(interval);
    }

    /**
     Returns an Observable which emits the same values as this Observable, but suppresses consecutive duplicate values.
     
     The value type must be equality-comparable.
     
     For example:
     
         Observable<int>::from({1, 2, 2, 2, 3, 4, 4, 6}).distinctUntilChanged(); // Emits: 1, 2, 3, 4, 6
     */
    Observable<T> distinctUntilChanged() const
    {
        return impl.distinctUntilChanged([](const any& lhs, const any& rhs) {
            // T must be equality-comparable
            return (lhs.get<T>() == rhs.get<T>());
        });
    }

    /**
     Returns an Observable which emits only one value: The `index`th value emitted by this Observable.
     */
    Observable<T> elementAt(int index) const
    {
        return impl.elementAt(index);
    }

    /**
     Returns an Observable that emits only those values from this Observable that pass a predicate function.
     */
    Observable<T> filter(const std::function<bool(const T&)>& predicate) const
    {
        return impl.filter([predicate](const any& value) {
            return predicate(value.get<T>());
        });
    }

    /**
     For each emitted value, calls `f` and subscribes to the Observable returned from `f`. The emitted values from all these returned Observables are *merged* (so they interleave).
     
     This Observable:
     
         Observable<String>::from({"Hello", "World"}).flatMap([](String s) {
             return Observable<String>::from({s.toLowerCase(), s.toUpperCase() + "!"});
         });
     
     Will emit the values: `"hello"`, `"HELLO!"`, `"world"` and `"WORLD!"`.
     
     @see Observable::merge, Observable::switchOnNext.
     */
    template<typename Function>
    Observable<typename CallResult<Function, T>::ValueType> flatMap(Function&& function, enable_if_t<IsObservable<CallResult<Function, T>>::value>* = 0) const
    {
        return impl.flatMap([function](const any& value) {
            return function(value.get<T>()).impl;
        });
    }

    /**
     For each value emitted by this Observable, call the function with that value and emit the result.
     
     If `f` returns an Observable, you can use Observable::switchOnNext afterwards.
     */
    template<typename Function>
    Observable<CallResult<Function, T>> map(Function&& function) const
    {
        return impl.map([function](const any& value) {
            return toAny(function(value.get<T>()));
        });
    }

    /**
     Merges the emitted values of this observable and the others into one Observable. The values are interleaved, depending on when the source Observables emit values.
     
     An error in one of the source Observables notifies the result Observable's `onError` immediately.
     */
    template<typename... Ts>
    Observable<T> merge(const Observable<Ts>&... others) const
    {
        static_assert(sizeof...(others) > 0, "Must pass at least one other Observable to merge.");
        static_assert(sizeof...(others) <= Impl::MaximumArity, "Too many Observables passed to concat.");
        static_assert(conjunction<std::is_same<T, Ts>...>::value, "All Observables must have the same type.");
        
        return impl.merge({others.impl...});
    }

    /**
     Begins with a `startValue`, and then applies `f` to all values emitted by this Observable, and returns the aggregate result as a single-element Observable sequence.
     */
    Observable<T> reduce(const T& startValue, const std::function<T(const T&, const T&)>& f) const
    {
        return impl.reduce(toAny(startValue), [f](const any& v1, const any& v2) {
            return toAny(f(v1.get<T>(), v2.get<T>()));
        });
    }

    /**
     Returns an Observable which checks every `interval` milliseconds whether this Observable has emitted any new values. If so, the returned Observable emits the latest value from this Observable.
     
     For example, this is useful when an Observable emits values very rapidly, but you only want to update a GUI component 25 times per second to reduce CPU load.
     
     The interval has millisecond resolution.
     */
    Observable<T> sample(const juce::RelativeTime& interval) const
    {
        return impl.sample(interval);
    }

    /**
     Calls a function `f` with the given `startValue` and the first value emitted by this Observable. The value returned from `f` is remembered. When the second value is emitted, `f` is called with the remembered value (called the *accumulator*) and the second emitted value. The returned value is remembered, until the third value is emitted, and so on.
     
     The first parameter to `f` is the accumulator, the second is the current value.
     */
    Observable<T> scan(const T& startValue, const std::function<T(const T&, const T&)>& f) const
    {
        return impl.scan(toAny(startValue), [f](const any& v1, const any& v2) {
            return toAny(f(v1.get<T>(), v2.get<T>()));
        });
    }

    /**
     Returns an Observable which suppresses emitting the first `numValues` values from this Observable.
     */
    Observable<T> skip(unsigned int numValues) const
    {
        return impl.skip(numValues);
    }

    /**
     Returns an Observable which suppresses emitting values from this Observable until the `other` Observable sequence emits a value.
     */
    template<typename U>
    Observable<T> skipUntil(const Observable<U>& other) const
    {
        return impl.skipUntil(other.impl);
    }

    /**
     Emits the given value(s) before beginning to emit the values in this Observable.
     */
    Observable<T> startWith(std::initializer_list<T> values) const
    {
        // Nothing to do if values is empty
        if (values.size() == 0)
            return *this;

        // Too many values:
        jassert(values.size() <= Impl::MaximumArity);

        juce::Array<any> anyValues;
        for (auto& value : values)
            anyValues.add(toAny(value));

        return impl.startWith(std::move(anyValues));
    }

    /**
     ​Returns an Observable that emits the values emitted by the Observables which this Observable emits.
     
     Therefore, it can only be used when this Observable emits Observables.
     */
    template<typename U = T>
    Observable<typename U::ValueType> switchOnNext(enable_if_t<std::is_same<U, T>::value && IsObservable<U>::value>* = 0) const
    {
        return impl.switchOnNext();
    }

    /**
     Returns an Observable that emits only the first `numValues` values from this Observable.
     */
    Observable<T> take(unsigned int numValues) const
    {
        return impl.take(numValues);
    }

    /**
     Returns an Observable that emits only the last `numValues` values from this Observable.
     */
    Observable<T> takeLast(unsigned int numValues) const
    {
        return impl.takeLast(numValues);
    }

    /**
     Emits values from this Observable until the `other` Observable sequence emits a value.
     */
    template<typename U>
    Observable<T> takeUntil(const Observable<U>& other) const
    {
        return impl.takeUntil(other.impl);
    }

    /**
     Emits values from the beginning of this Observable as long as the given `predicate` returns `true`.
     
     The predicate is called on each value emitted by this Observable, until it returns `false`.
     */
    Observable<T> takeWhile(const std::function<bool(const T&)>& predicate) const
    {
        return impl.takeWhile([predicate](const any& value) {
            return predicate(value.get<T>());
        });
    }

    ///@{
    /**
     Returns an Observable that emits whenever a value is emitted by **this Observable**. It combines the latest value from each Observable via the given function and emits the result of this function.
     
     This is different from Observable::combineLatest because it only emits when this Observable emits a value (not when one of the `others` emits a value).
     */
    template<typename... Ts>
    Observable<std::tuple<T, Ts...>> withLatestFrom(const Observable<Ts>&... others) const
    {
        return withLatestFrom(std::make_tuple<const T&, const Ts&...>, others...);
    }
    /// \overload
    template<typename... Ts, typename Function>
    Observable<CallResult<Function, T, Ts...>> withLatestFrom(Function&& function, const Observable<Ts>&... others) const
    {
        static_assert(sizeof...(Ts) > 0, "Must pass at least one other Observable to withLatestFrom.");
        static_assert(sizeof...(Ts) <= Impl::MaximumArity, "Too many Observables passed to withLatestFrom.");

        const std::function<any(const any&, const typename any_args<Ts>::type&...)> untypedFunction = [function](const any& v, const typename any_args<Ts>::type&... vs) {
            return toAny(function(v.get<T>(), vs.template get<Ts>()...));
        };

        return impl.withLatestFrom({ others.impl... }, toAny(untypedFunction));
    }
    ///@}

    ///@{
    /**
     Returns an Observable that emits whenever each of the Observables have emitted a new value. It combines the **latest** value from each Observable via the given function and emits the result of this function.
     
     It applies this function in strict sequence, so the first value emitted by the returned Observable is the result of `f` applied to the first value emitted by this Observable and the first value emitted by `o1`; the second value emitted by the returned Observable is the result of `f` applied to the second value emitted by this Observable and the second value emitted by `o1`; and so on.
     
     The returned Observable only emits as many values as the number of values emitted by the source Observable that emits the fewest values.
     */
    template<typename... Ts>
    Observable<std::tuple<T, Ts...>> zip(const Observable<Ts>&... others) const
    {
        return zip(std::make_tuple<const T&, const Ts&...>, others...);
    }
    /// \overload
    template<typename... Ts, typename Function>
    Observable<CallResult<Function, T, Ts...>> zip(Function&& function, const Observable<Ts>&... others) const
    {
        static_assert(sizeof...(Ts) > 0, "Must pass at least one other Observable to zip.");
        static_assert(sizeof...(Ts) <= Impl::MaximumArity, "Too many Observables passed to zip.");

        const std::function<any(const any&, const typename any_args<Ts>::type&...)> untypedFunction = [function](const any& v, const typename any_args<Ts>::type&... vs) {
            return toAny(function(v.get<T>(), vs.template get<Ts>()...));
        };

        return impl.zip({ others.impl... }, toAny(untypedFunction));
    }
        ///@}


#pragma mark - Scheduling
    /**
     Returns an Observable that will be observed on a specified scheduler, for example the JUCE Message Thread or a background thread.
     
     When you apply Observable::map to the returned Observable, the map function will run on the specified scheduler.
     
     For example:
     
         Observable<int>::range(1, 1000, 1)
             .observeOn(Scheduler::newThread())
             .map([](double d){ return std::sqrt(d); }) // This lambda is called on a new thread
             .observeOn(Scheduler::messageThread())
             .subscribe([&](double squareRoot) { }); // This lambda is called on the message thread
     
     @see Scheduler::messageThread, Scheduler::backgroundThread and Scheduler::newThread
     */
    Observable<T> observeOn(const Scheduler& scheduler) const
    {
        return impl.observeOn(*scheduler.impl);
    }


#pragma mark - Misc
    /**
     Blocks until the Observable has completed, then returns an Array of all emitted values.
     
     Be careful when you use this on the message thread: If the Observable needs to process something *asynchronously* on the message thread, calling this will deadlock.
     
     ​ **If you don't pass an `onError` handler, an exception inside the Observable will terminate your app.**
     */
    juce::Array<T> toArray(const std::function<void(std::exception_ptr)>& onError = Impl::TerminateOnError) const
    {
        juce::Array<T> values;

        for (const any& v : impl.toArray(onError))
            values.add(v.get<T>());

        return values;
    }

    /// Covariant constructor: If `U` is convertible to `T`, then an `Observable<U>` is convertible to an `Observable<T>`.
    template<typename U>
    Observable(const Observable<U>& other, enable_if_t<std::is_convertible<U, T>::value>* = 0)
    : Observable(other.impl.map([](const any& u) { return toAny(static_cast<T>(u.get<U>())); }))
    {}

private:
    template<typename U>
    friend class Observable;
    template<typename U>
    friend class Subject;

    Impl impl;

    Observable(const Impl& impl)
    : impl(impl)
    {}

    // Calls the any() constructor, but for Observable<T> it stores the ObservableImpl
    template<typename U>
    static any toAny(const U& u, enable_if_t<!IsObservable<U>::value>* = 0)
    {
        return any(u);
    }
    template<typename U>
    static any toAny(const U& u, enable_if_t<IsObservable<U>::value>* = 0)
    {
        return any(u.impl);
    }

    // any_args<Ts...>::type is a parameter pack with the same length as Ts, where all types are any.
    template<typename>
    struct any_args
    {
        typedef any type;
    };

    JUCE_LEAK_DETECTOR(Observable)
};
