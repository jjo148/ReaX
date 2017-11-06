#pragma once

template<typename T>
class Observer;

/**
 An Observable emits values of a given type.
 
 Observers can subscribe to Observable​s, to be notified whenever the Observable emits a value.
 
 For an introduction to Observables, please refer to http://reactivex.io/documentation/observable.html.
 */
template<typename T = void>
class Observable
{
public:
#pragma mark - Helpers
    typedef T ItemType;

    typedef detail::ObservableImpl Impl;
    typedef detail::any any;

    ///@{
    /// Determines whether a given type is an Observable. 
    template<typename U>
    struct IsObservable : std::false_type
    {
    };

    template<typename U>
    struct IsObservable<Observable<U>> : std::true_type
    {
    };
    ///@}

    /// The return type of calling a function of type Transform with parameters of types Args. 
    template<typename Transform, typename... Args>
    using CallResult = typename std::result_of<Transform(Args...)>::type;

    
#pragma mark - Creation
    /**
     Creates an Observable that doesn't emit any items and notifies onComplete immediately.
     
     This is the same as Observable<T>::empty().
     */
    Observable<T>()
    : impl(Impl::empty())
    {}

    
#pragma mark - Subscription
    ///@{
    /**
     Subscribes to an Observable, to receive values it emits.
     
     The **onNext** function is called whenever the Observable emits a new item. It may be called synchronously before subscribe() returns.
     
     The **onError** function is called when the Observable has failed to generate the expected data, or has encountered some other error. If onError is called, the Observable will not make any more calls. **If you don't pass an onError handler, an exception inside the Observable will terminate your app.**
     
     The **onCompleted** function is called exactly once to notify that the Observable has generated all data and will not emit any more items.
     
     The returned Subscription can be used to unsubscribe from the Observable, to stop receiving values from it. **You will keep receiving values until you call Subscription::dispose, or until the Observable source is destroyed**. The best way is to use a DisposeBag, which automatically unsubscribes when it is destroyed.
     
     Example:
     
         DisposeBag disposeBag;
         Observable::from<int>({4, 17}).subscribe([](int i) {
             std::cout << i << " ";
         }).disposedBy(disposeBag); // Output: 4 17
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
     Subscribes an Observer to an Observable. The Observer is notified whenever the Observable emits an item, or notifies an onError / onCompleted.
     
     The returned Subscription can be used to unsubscribe the Observer, so it stops being notified by this Observable. **The Observer keeps receiving values until you call Subscription::dispose, or until the Observable source is destroyed**. The best way is to use a DisposeBag, which automatically unsubscribes when it is destroyed.
     */
    template<typename U>
    typename std::enable_if<std::is_convertible<T, U>::value, Subscription>::type subscribe(const Observer<U>& observer) const
    {
        // Convert items from T to U, using implicit conversion, constructors, whatever works
        auto converted = impl.map([](const any& t) {
            return toAny<U>(t.get<T>());
        });

        return converted.subscribe(observer.impl);
    }
    ///@}

    
#pragma mark - Operators
    ///@{
    /**
     Returns an Observable that emits **whenever** an item is emitted by either this Observable **or** one of the  `others`. It combines the **latest** item from each Observable via the given function and emits the result of this function.
     
     This is different from Observable::withLatestFrom because it emits whenever this Observable or one of the  `others` emits an item.
     
     @see Observable::withLatestFrom
     */
    template<typename... Ts>
    Observable<std::tuple<T, Ts...>> combineLatest(const Observable<Ts>&... others) const
    {
        return combineLatest(std::make_tuple<T, Ts...>, others...);
    }

    template<typename... Ts, typename Transform>
    Observable<CallResult<Transform, T, Ts...>> combineLatest(Transform&& transform, const Observable<Ts>&... others) const
    {
        static_assert(sizeof...(Ts) > 0, "Must pass at least one other Observable to combineLatest.");
        static_assert(sizeof...(Ts) <= Impl::MaximumArity, "Too many Observables passed to combineLatest.");

        const std::function<any(const any&, const typename any_args<Ts>::type&...)> untypedTransform = [transform](const any& v, const typename any_args<Ts>::type&... vs) {
            return toAny(transform(v.get<T>(), vs.template get<Ts>()...));
        };

        return impl.combineLatest({ others.impl... }, toAny(untypedTransform));
    }
    ///@}

    /**
     Returns an Observable that first emits the items from this Observable, then from the first in the `others` list, then from the second, and so on.
     
     It only subscribes to the first in the `others` list when this Observable has completed. And only subscribes to the second when the first has completed, and so on.
     */
    Observable<T> concat(std::initializer_list<Observable<T>> others) const
    {
        // Must pass at least one other Observable:
        jassert(others.size() > 0);

        // Too many Observables passed to concat:
        jassert(others.size() <= Impl::MaximumArity);

        juce::Array<Impl> otherImpls;
        for (auto& other : others)
            otherImpls.add(other.impl);

        return impl.concat(otherImpls);
    }

    /**
     Returns an Observable which emits if `interval` has passed without this Observable emitting an item. The returned Observable emits the latest item from this Observable.
     
     It's like the instant search in a search engine: Search suggestions are only loaded if the user hasn't pressed a key for a short period of time.
     
     The interval has millisecond resolution.
     */
    Observable<T> debounce(const juce::RelativeTime& interval) const
    {
        return impl.debounce(interval);
    }

    /**
     Returns an Observable which emits the same items as this Observable, but suppresses consecutive duplicate items.
     
     For example:
     
         Observable::from<int>({1, 2, 2, 2, 3, 4, 4, 6}).distinctUntilChanged(); // Emits: 1, 2, 3, 4, 6
     
     If T is comparable using ==, this is used to determine whether two items are equal. Otherwise, the items are compared by their addresses.
     
     If, for some reason, the custom type T doesn't have operator==, you can pass a custom equality function.
     */
    Observable<T> distinctUntilChanged(const std::function<bool(const T&, const T&)>& equals = std::equal_to<T>()) const
    {
        return impl.distinctUntilChanged([equals](const any& lhs, const any& rhs) {
            return equals(lhs.get<T>(), rhs.get<T>());
        });
    }

    /**
     Returns an Observable which emits only one item: The `index`th item emitted by this Observable.
     */
    Observable<T> elementAt(int index) const
    {
        return impl.elementAt(index);
    }

    /**
     Returns an Observable that emits only those items from this Observable that pass a predicate function.
     */
    Observable<T> filter(const std::function<bool(const T&)>& predicate) const
    {
        return impl.filter([predicate](const any& item) {
            return predicate(item.get<T>());
        });
    }

    /**
     For each emitted item, calls `f` and subscribes to the Observable returned from `f`. The emitted items from all these returned Observables are *merged* (so they interleave).
     
     This Observable:
     
         Observable::from<String>({"Hello", "World"}).flatMap([](String s) {
             return Observable::from<String>({s.toLowerCase(), s.toUpperCase() + "!"});
         });
     
     Will emit the items: `"hello"`, `"HELLO!"`, `"world"` and `"WORLD!"`.
     
     @see Observable::merge, Observable::switchOnNext.
     */
    template<typename Transform>
    typename std::enable_if<IsObservable<CallResult<Transform, T>>::value, Observable<typename CallResult<Transform, T>::ItemType>>::type flatMap(Transform&& transform) const
    {
        return impl.flatMap([transform](const any& item) {
            return transform(item.get<T>()).impl;
        });
    }

    /**
     For each item emitted by this Observable, call the function with that item and emit the result.
     
     If `f` returns an Observable, you can use Observable::switchOnNext afterwards.
     */
    template<typename Transform>
    Observable<CallResult<Transform, T>> map(Transform&& transform) const
    {
        return impl.map([transform](const any& item) {
            return toAny(transform(item.get<T>()));
        });
    }

    /**
     Merges the emitted items of this observable and the others into one Observable. The items are interleaved, depending on when the source Observables emit items.
     
     An error in one of the source Observables notifies the result Observable's `onError` immediately.
     */
    Observable<T> merge(std::initializer_list<Observable<T>> others) const
    {
        // Must pass at least one other Observable:
        jassert(others.size() > 0);

        // Too many Observables:
        jassert(others.size() <= Impl::MaximumArity);

        juce::Array<Impl> otherImpls;
        for (auto& other : others)
            otherImpls.add(other.impl);

        return impl.merge(otherImpls);
    }

    /**
     Begins with a `startValue`, and then applies `f` to all items emitted by this Observable, and returns the aggregate result as a single-element Observable sequence.
     */
    Observable<T> reduce(const T& startValue, const std::function<T(const T&, const T&)>& f) const
    {
        return impl.reduce(toAny(startValue), [f](const any& v1, const any& v2) {
            return toAny(f(v1.get<T>(), v2.get<T>()));
        });
    }

    /**
     Returns an Observable which checks every `interval` milliseconds whether this Observable has emitted any new items. If so, the returned Observable emits the latest item from this Observable.
     
     For example, this is useful when an Observable emits items very rapidly, but you only want to update a GUI component 25 times per second to reduce CPU load.
     
     The interval has millisecond resolution.
     */
    Observable<T> sample(const juce::RelativeTime& interval) const
    {
        return impl.sample(interval);
    }

    /**
     Calls a function `f` with the given `startValue` and the first item emitted by this Observable. The value returned from `f` is remembered. When the second item is emitted, `f` is called with the remembered value (called the *accumulator*) and the second emitted item. The returned item is remembered, until the third item is emitted, and so on.
     
     The first parameter to `f` is the accumulator, the second is the current item.
     */
    Observable<T> scan(const T& startValue, const std::function<T(const T&, const T&)>& f) const
    {
        return impl.scan(toAny(startValue), [f](const any& v1, const any& v2) {
            return toAny(f(v1.get<T>(), v2.get<T>()));
        });
    }

    /**
     Returns an Observable which suppresses emitting the first `numItems` items from this Observable.
     */
    Observable<T> skip(unsigned int numItems) const
    {
        return impl.skip(numItems);
    }

    /**
     Returns an Observable which suppresses emitting items from this Observable until the `other` Observable sequence emits an item.
     */
    template<typename T1>
    Observable<T> skipUntil(const Observable<T1>& other) const
    {
        return impl.skipUntil(other.impl);
    }

    /**
     Emits the given item(s) before beginning to emit the items in this Observable.
     */
    Observable<T> startWith(std::initializer_list<T> items) const
    {
        // Must pass at least one item:
        jassert(items.size() > 0);

        // Too many items:
        jassert(items.size() <= Impl::MaximumArity);

        juce::Array<any> anyItems;
        for (auto& item : items)
            anyItems.add(toAny(item));

        return impl.startWith(std::move(anyItems));
    }

    /**
     ​Returns an Observable that emits the items emitted by the Observables which this Observable emits.
     
     Therefore, it can only be used when this Observable emits Observables.
     */
    template<typename U = T>
    typename std::enable_if<IsObservable<U>::value, U>::type switchOnNext() const
    {
        return impl.switchOnNext();
    }

    /**
     Returns an Observable that emits only the first `numItems` items from this Observable.
     */
    Observable<T> take(unsigned int numItems) const
    {
        return impl.take(numItems);
    }

    /**
     Returns an Observable that emits only the last `numItems` items from this Observable.
     */
    Observable<T> takeLast(unsigned int numItems) const
    {
        return impl.takeLast(numItems);
    }

    /**
     Emits items from this Observable until the `other` Observable sequence emits an item.
     */
    template<typename U>
    Observable<T> takeUntil(const Observable<U>& other) const
    {
        return impl.takeUntil(other.impl);
    }

    /**
     Emits items from the beginning of this Observable as long as the given `predicate` returns `true`.
     
     The predicate is called on each item emitted by this Observable, until it returns `false`.
     */
    Observable<T> takeWhile(const std::function<bool(const T&)>& predicate) const
    {
        return impl.takeWhile([predicate](const any& item) {
            return predicate(item.get<T>());
        });
    }

    ///@{
    /**
     Returns an Observable that emits whenever an item is emitted by this Observable. It combines the latest item from each Observable via the given function and emits the result of this function.
     
     This is different from Observable::combineLatest because it only emits when this Observable emits an item (not when o1, o2, … emit items).
     */
    template<typename... Ts>
    Observable<std::tuple<T, Ts...>> withLatestFrom(const Observable<Ts>&... others) const
    {
        return withLatestFrom(std::make_tuple<T, Ts...>, others...);
    }

    template<typename... Ts, typename Transform>
    Observable<CallResult<Transform, T, Ts...>> withLatestFrom(Transform&& transform, const Observable<Ts>&... others) const
    {
        static_assert(sizeof...(Ts) > 0, "Must pass at least one other Observable to withLatestFrom.");
        static_assert(sizeof...(Ts) <= Impl::MaximumArity, "Too many Observables passed to withLatestFrom.");

        const std::function<any(const any&, const typename any_args<Ts>::type&...)> untypedTransform = [transform](const any& v, const typename any_args<Ts>::type&... vs) {
            return toAny(transform(v.get<T>(), vs.template get<Ts>()...));
        };

        return impl.withLatestFrom({ others.impl... }, toAny(untypedTransform));
    }
    ///@}

    ///@{
    /**
     Returns an Observable that emits **whenever** an item is emitted by either this Observable **or** o1, o2, …. It combines the **latest** item from each Observable via the given function and emits the result of this function.
     
     It applies this function in strict sequence, so the first item emitted by the returned Observable is the result of `f` applied to the first item emitted by this Observable and the first item emitted by `o1`; the second item emitted by the returned Observable is the result of `f` applied to the second item emitted by this Observable and the second item emitted by `o1`; and so on.
     
     The returned Observable only emits as many items as the number of items emitted by the source Observable that emits the fewest items.
     */
    template<typename... Ts>
    Observable<std::tuple<T, Ts...>> zip(const Observable<Ts>&... others) const
    {
        return zip(std::make_tuple<T, Ts...>, others...);
    }

    template<typename... Ts, typename Transform>
    Observable<CallResult<Transform, T, Ts...>> zip(Transform&& transform, const Observable<Ts>&... others) const
    {
        static_assert(sizeof...(Ts) > 0, "Must pass at least one other Observable to zip.");
        static_assert(sizeof...(Ts) <= Impl::MaximumArity, "Too many Observables passed to zip.");

        const std::function<any(const any&, const typename any_args<Ts>::type&...)> untypedTransform = [transform](const any& v, const typename any_args<Ts>::type&... vs) {
            return toAny(transform(v.get<T>(), vs.template get<Ts>()...));
        };

        return impl.zip({ others.impl... }, toAny(untypedTransform));
    }
        ///@}


#pragma mark - Scheduling
    /**
     Returns an Observable that will be observed on a specified scheduler, for example the JUCE Message Thread or a background thread.
     
     When you apply Observable::map to the returned Observable, the map function will run on the specified scheduler.
     
     For example:
     
     Observable::range(Range<double>(1, 1000), 1)
     .observeOn(Scheduler::newThread())
     .map([](double d){ return std::sqrt(d); }) // The lambda will be called on a new thread
     .observeOn(Scheduler::messageThread())
     .subscribe([&](double squareRoot) { }); // The lambda will be called on the message thread
     
     @see Scheduler::messageThread, Scheduler::backgroundThread and Scheduler::newThread
     */
    Observable<T> observeOn(const Scheduler& scheduler) const
    {
        return impl.observeOn(*scheduler.impl);
    }


#pragma mark - Misc
    /**
     Blocks until the Observable has completed, then returns an Array of all emitted items.
     
     Be careful when you use this on the message thread: If the Observable needs to process something *asynchronously* on the message thread, calling this will deadlock.
     
     ​ **If you don't pass an onError handler, an exception inside the Observable will terminate your app.**
     */
    juce::Array<T> toArray(const std::function<void(std::exception_ptr)>& onError = Impl::TerminateOnError) const
    {
        juce::Array<T> items;

        for (const any& v : impl.toArray(onError))
            items.add(v.get<T>());

        return items;
    }

    /// Covariant constructor: If U is convertible to T, an Observable<U> is convertible to an Observable<T>. 
    template<typename U>
    Observable(const Observable<U>& other, typename std::enable_if<std::is_convertible<U, T>::value>::type* = 0)
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
    static typename std::enable_if<!IsObservable<U>::value, any>::type toAny(U&& u)
    {
        return any(std::forward<U>(u));
    }
    template<typename U>
    static typename std::enable_if<IsObservable<U>::value, any>::type toAny(U&& u)
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

template<>
class Observable<void>
{
public:
    typedef detail::ObservableImpl Impl;
    typedef detail::any any;
    
    
#pragma mark - Creation
    
    /**
     Creates an Observable which emits values from an Observer on each subscription.
     
     In the onSubscribe function, you get an Observer. You can call Observer::onNext on it to emit values from the Observable.
     */
    template<typename T>
    static Observable<T> create(const std::function<void(const Observer<T>&)>& onSubscribe)
    {
        return Impl::create([onSubscribe](detail::ObserverImpl&& impl) {
            onSubscribe(Observer<T>(std::move(impl)));
        });
    }
    
    /**
     Creates a new Observable for each subscriber, by calling the `factory` function on each new subscription.
     */
    template<typename T>
    static Observable<T> defer(const std::function<Observable<T>()>& factory)
    {
        return Impl::defer([factory]() {
            return factory().impl;
        });
    }
    
    /**
     Creates an Observable that doesn't emit any items and notifies onComplete immediately.
     */
    template<typename T>
    static Observable<T> empty()
    {
        return Impl::empty();
    }
    
    /**
     Creates an Observable which doesn't emit any items, and immediately notifies onError.
     */
    template<typename T>
    static Observable<T> error(const std::exception& error)
    {
        return Impl::error(error);
    }
    
    /**
     Creates an Observable that immediately emits the items from the given Array.
     
     Note that you can also pass an initializer list, like this:
     
     Observable::from({"Hello", "Test"})
     
     Observable::from({var(3), var("four")})
     */
    template<typename T>
    static Observable<T> from(const juce::Array<T>& array)
    {
        juce::Array<any> items;
        
        for (auto& item : array)
            items.add(Observable<T>::toAny(item));
        
        return Impl::from(std::move(items));
    }
    
    /**
     Creates an Observable from a given JUCE Value. The returned Observable **only emits items until it is destroyed**, so you are responsible for managing its lifetime. Or use Reactive<Value>, which will handle this.
     
     The returned Observable notifies the onComplete handler when it's destroyed. @see Observable::subscribe
     
     When calling Value::setValue, it notifies asynchronously. So **the returned Observable emits the new value asynchronously.** If you call setValue immediately before destroying the returned Observable, the new value will not be emitted.
     */
    static Observable<juce::var> fromValue(juce::Value value)
    {
        return Impl::fromValue(value);
    }
    
    /**
     Returns an Observable that emits one item every `interval`, starting at the time of subscription (where the first item is emitted). The emitted items are `1`, `2`, `3`, and so on.
     
     The Observable emits endlessly, but you can use Observable::take to get a finite number of items (for example).
     
     The interval has millisecond resolution.
     */
    static Observable<int> interval(const juce::RelativeTime& interval)
    {
        return Impl::interval(interval);
    }
    
    /**
     Creates an Observable which emits a single item.
     
     The value is emitted immediately on each new subscription.
     */
    template<typename T>
    static Observable<T> just(const T& value)
    {
        return Impl::just(Observable<T>::toAny(value));
    }
    
    /**
     Creates an Observable that never emits any events and never terminates.
     */
    template<typename T>
    static Observable<T> never()
    {
        return Impl::never();
    }
    
    /**
     Creates an Observable which emits a range of items, starting at `first` to (and including) `last`. It completes after emitting the `last` item.
     
     ​ **Throws an exception if first > last.**
     
     For example:
     
     Observable::range(3, 7, 3) // {3, 6, 7}
     Observable::range(17.5, 22.8, 2) // {17.5, 19.5, 21.5, 22.8}
     */
    template<typename T>
    static Observable<T> range(T first, T last, unsigned int step = 1)
    {
        static_assert(std::is_integral<T>::value, "first and last must be integral or floating-point.");
        return Impl::integralRange(first, last, step);
    }
    
    /**
     Creates an Observable which emits a given item repeatedly.
     
     An optional `times` parameter specifies how often the item should be repeated. If omitted, the item will is repeated indefinitely.
     */
    template<typename T>
    static Observable<T> repeat(const T& item)
    {
        return Impl::repeat(Observable<T>::toAny(item));
    }
    /// \overload 
    template<typename T>
    static Observable<T> repeat(const T& item, unsigned int times)
    {
        return Impl::repeat(Observable<T>::toAny(item), times);
    }
};


template<>
inline Observable<float> Observable<void>::range(float first, float last, unsigned int step)
{
    return Impl::floatRange(first, last, step);
}

template<>
inline Observable<double> Observable<void>::range(double first, double last, unsigned int step)
{
    return Impl::doubleRange(first, last, step);
}
