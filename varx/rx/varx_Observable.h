#pragma once

class Scheduler;

template<typename T>
class Observer;

template<typename T = void>
class Observable
{
public:
    typedef T ItemType;

    typedef detail::ObservableImpl Impl;
    typedef detail::any any;

    template<typename U>
    struct IsObservable : std::false_type
    {
    };

    template<typename U>
    struct IsObservable<Observable<U>> : std::true_type
    {
    };

#pragma mark - Creation
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
     Creates an Observable that doesn't emit any items and notifies onComplete immediately.
     */
    static Observable<T> empty()
    {
        return Impl::empty();
    }

    /**
     Creates an Observable which doesn't emit any items, and immediately notifies onError.
     */
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
    static Observable<T> from(const juce::Array<T>& array)
    {
        juce::Array<any> items;

        for (auto& item : array)
            items.add(toAny(item));

        return Impl::from(std::move(items));
    }

    /**
     Creates an Observable which emits a single item.
     
     The value is emitted immediately on each new subscription.
     */
    static Observable<T> just(const T& value)
    {
        return Impl::just(toAny(value));
    }

    /**
     Creates an Observable that never emits any events and never terminates.
     */
    static Observable<T> never()
    {
        return Impl::never();
    }

    /**
     Creates an Observable which emits a given item repeatedly.
     
     An optional `times` parameter specifies how often the item should be repeated. If omitted, the item will is repeated indefinitely.
     */
    static Observable<T> repeat(const T& item)
    {
        return Impl::repeat(toAny(item));
    }
    /** \overload */
    static Observable<T> repeat(const T& item, unsigned int times)
    {
        return Impl::repeat(toAny(item), times);
    }


#pragma mark - Subscription
    ///@{
    /**
     Subscribes to an Observable, to receive values it emits.
     
     The **onNext** function is called whenever the Observable emits a new item. It may be called synchronously before subscribe() returns.
     
     The **onError** function is called when the Observable has failed to generate the expected data, or has encountered some other error. If onError is called, the Observable will not make any more calls. **If you don't pass an onError handler, an exception inside the Observable will terminate your app.**
     
     The **onCompleted** function is called exactly once to notify that the Observable has generated all data and will not emit any more items.
     
     The returned Disposable can be used to unsubscribe from the Observable, to stop receiving values from it. **You will keep receiving values until you call Disposable::dispose, or until the Observable source is destroyed**. The best way is to use a DisposeBag, which automatically unsubscribes when it is destroyed.
     */
    Disposable subscribe(const std::function<void(const T&)>& onNext,
                         const std::function<void(Error)>& onError = Impl::TerminateOnError,
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
     
     The returned Disposable can be used to unsubscribe the Observer, so it stops being notified by this Observable. **The Observer keeps receiving values until you call Disposable::dispose, or until the Observable source is destroyed**. The best way is to use a DisposeBag, which automatically unsubscribes when it is destroyed.
     */
    template<typename U>
    typename std::enable_if<std::is_convertible<T, U>::value, Disposable>::type subscribe(const Observer<U>& observer) const
    {
        return impl.subscribe(observer.impl);
    }
        ///@}


#pragma mark - Helper Functions
    template<typename Transform, typename... Args>
    using CallResult = decltype(std::declval<Transform>()(std::declval<Args>()...));

#pragma mark - Operators
    ///@{
    /**
     Returns an Observable that emits **whenever** an item is emitted by either this Observable **or** o1, o2, …. It combines the **latest** item from each Observable via the given function and emits the result of this function.
     
     This is different from Observable::withLatestFrom because it emits whenever this Observable or o1, o2, … emits an item.
     
     @see Observable::withLatestFrom
     */
#warning Ugly, should be changed
    template<typename T1, typename Transform = std::function<std::tuple<T, T1>(const T, const T1)>>
    Observable<CallResult<Transform, T, T1>> combineLatest(const Observable<T1>& o1, Transform&& transform = std::make_tuple<T, T1>) const
    {
        return impl.combineLatest(o1.impl, [transform](const any& v0, const any& v1) {
            return toAny(transform(v0.get<T>(), v1.get<T1>()));
        });
    }

    /** \overload */
    template<typename T1, typename T2, typename Transform = std::function<std::tuple<T, T1, T2>(const T, const T1, const T2)>>
    Observable<CallResult<Transform, T, T1, T2>> combineLatest(const Observable<T1>& o1, const Observable<T2>& o2, Transform&& transform = std::make_tuple<T, T1, T2>) const
    {
        return impl.combineLatest(o1.impl, o2.impl, [transform](const any& v0, const any& v1, const any& v2) {
            return toAny(transform(v0.get<T>(), v1.get<T1>(), v2.get<T2>()));
        });
    }

    /** \overload */
    template<typename T1, typename T2, typename T3, typename Transform = std::function<std::tuple<T, T1, T2, T3>(const T, const T1, const T2, const T3)>>
    Observable<CallResult<Transform, T, T1, T2, T3>> combineLatest(const Observable<T1>& o1, const Observable<T2>& o2, const Observable<T3>& o3, Transform&& transform = std::make_tuple<T, T1, T3>) const
    {
        return impl.combineLatest(o1.impl, o2.impl, o3.impl, [transform](const any& v0, const any& v1, const any& v2, const any& v3) {
            return toAny(transform(v0.get<T>(), v1.get<T1>(), v2.get<T2>(), v3.get<T3>()));
        });
    }

    /** \overload */
    template<typename T1, typename T2, typename T3, typename T4, typename Transform>
    Observable<CallResult<Transform, T, T1, T2, T3, T4>> combineLatest(const Observable<T1>& o1, const Observable<T2>& o2, const Observable<T3>& o3, const Observable<T4>& o4, Transform&& transform) const
    {
        return impl.combineLatest(o1.impl, o2.impl, o3.impl, o4.impl, [transform](const any& v0, const any& v1, const any& v2, const any& v3, const any& v4) {
            return toAny(transform(v0.get<T>(), v1.get<T1>(), v2.get<T2>(), v3.get<T3>(), v4.get<T4>()));
        });
    }

    /** \overload */
    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename Transform>
    Observable<CallResult<Transform, T, T1, T2, T3, T4, T5>> combineLatest(const Observable<T1>& o1, const Observable<T2>& o2, const Observable<T3>& o3, const Observable<T4>& o4, const Observable<T5>& o5, Transform&& transform) const
    {
        return impl.combineLatest(o1.impl, o2.impl, o3.impl, o4.impl, o5.impl, [transform](const any& v0, const any& v1, const any& v2, const any& v3, const any& v4, const any& v5) {
            return toAny(transform(v0.get<T>(), v1.get<T1>(), v2.get<T2>(), v3.get<T3>(), v4.get<T4>(), v5.get<T5>()));
        });
    }

    /** \overload */
    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename Transform>
    Observable<CallResult<Transform, T, T1, T2, T3, T4, T5, T6>> combineLatest(const Observable<T1>& o1, const Observable<T2>& o2, const Observable<T3>& o3, const Observable<T4>& o4, const Observable<T5>& o5, const Observable<T6>& o6, Transform&& transform) const
    {
        return impl.combineLatest(o1.impl, o2.impl, o3.impl, o4.impl, o5.impl, o6.impl, [transform](const any& v0, const any& v1, const any& v2, const any& v3, const any& v4, const any& v5, const any& v6) {
            return toAny(transform(v0.get<T>(), v1.get<T1>(), v2.get<T2>(), v3.get<T3>(), v4.get<T4>(), v5.get<T5>(), v6.get<T6>()));
        });
    }

    /** \overload */
    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename Transform>
    Observable<CallResult<Transform, T, T1, T2, T3, T4, T5, T6, T7>> combineLatest(const Observable<T1>& o1, const Observable<T2>& o2, const Observable<T3>& o3, const Observable<T4>& o4, const Observable<T5>& o5, const Observable<T6>& o6, const Observable<T7>& o7, Transform&& transform) const
    {
        return impl.combineLatest(o1.impl, o2.impl, o3.impl, o4.impl, o5.impl, o6.impl, o7.impl, [transform](const any& v0, const any& v1, const any& v2, const any& v3, const any& v4, const any& v5, const any& v6, const any& v7) {
            return toAny(transform(v0.get<T>(), v1.get<T1>(), v2.get<T2>(), v3.get<T3>(), v4.get<T4>(), v5.get<T5>(), v6.get<T6>(), v7.get<T7>()));
        });
    }
    ///@}

    ///@{
    /**
     Returns an Observable that first emits the items from this Observable, then from o1, then from o2, and so on.
     
     It only subscribes to o1 when this Observable has completed. And only subscribes to o2 when o1 has completed, and so on.
     */
    Observable<T> concat(const Observable<T>& o1) const
    {
        return impl.concat(o1.impl);
    }
    ///@}

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
     
     If you just use distinctUntilChanged() without a template parameter, it uses juce::any::operator== to determine whether two items are equal.
     
     **If you are using VARX_DEFINE_VARIANT_CONVERTER to wrap a custom type T into a any, you should call distinctUntilChanged<T>(), declaring your custom type T.** This way, it uses T::operator== to determine whether two items are equal.
     
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
     
     Observable::from({"Hello", "World"}).flatMap([](String s) {
     return Observable::from({s.toLowerCase(), s.toUpperCase() + "!"});
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

    ///@{
    /**
     Merges the emitted items of this observable and o1, o2, … into one Observable. The items are interleaved, depending on when the source Observables emit items.
     
     An error in one of the source Observables notifies the result Observable's `onError` immediately.
     */
    Observable<T> merge(const Observable<T>& o1) const
    {
        return impl.merge(o1.impl);
    }

    ///@}

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

    ///@{
    /**
     Emits the given item(s) before beginning to emit the items in this Observable.
     */
    Observable<T> startWith(const T& v1) const
    {
        return impl.startWith(toAny(v1));
    }

    /** \overload */
    Observable<T> startWith(const T& v1, const T& v2) const
    {
        return impl.startWith(toAny(v1), toAny(v2));
    }

    /** \overload */
    Observable<T> startWith(const T& v1, const T& v2, const T& v3) const
    {
        return impl.startWith(toAny(v1), toAny(v2), toAny(v3));
    }

    /** \overload */
    Observable<T> startWith(const T& v1, const T& v2, const T& v3, const T& v4) const
    {
        return impl.startWith(toAny(v1), toAny(v2), toAny(v3), toAny(v4));
    }

    /** \overload */
    Observable<T> startWith(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5) const
    {
        return impl.startWith(toAny(v1), toAny(v2), toAny(v3), toAny(v4), toAny(v5));
    }

    /** \overload */
    Observable<T> startWith(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5, const T& v6) const
    {
        return impl.startWith(toAny(v1), toAny(v2), toAny(v3), toAny(v4), toAny(v5), toAny(v6));
    }

    /** \overload */
    Observable<T> startWith(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5, const T& v6, const T& v7) const
    {
        return impl.startWith(toAny(v1), toAny(v2), toAny(v3), toAny(v4), toAny(v5), toAny(v6), toAny(v7));
    }
    ///@}

    /**
     ​ **This must only be called if this Observable emits Observables**.
     
     Returns an Observable that emits the items emitted by the Observables which this Observable emits.
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
    template<typename T1, typename Transform = std::function<std::tuple<T, T1>(const T, const T1)>>
    Observable<CallResult<Transform, T, T1>> withLatestFrom(const Observable<T1>& o1, Transform&& transform = std::make_tuple<T, T1>) const
    {
        return impl.withLatestFrom(o1.impl, [transform](const any& v0, const any& v1) {
            return toAny(transform(v0.get<T>(), v1.get<T1>()));
        });
    }

    /** \overload */
    template<typename T1, typename T2, typename Transform>
    Observable<CallResult<Transform, T, T1, T2>> withLatestFrom(const Observable<T1>& o1, const Observable<T2>& o2, Transform&& transform) const
    {
        return impl.withLatestFrom(o1.impl, o2.impl, [transform](const any& v0, const any& v1, const any& v2) {
            return toAny(transform(v0.get<T>(), v1.get<T1>(), v2.get<T2>()));
        });
    }

    /** \overload */
    template<typename T1, typename T2, typename T3, typename Transform>
    Observable<CallResult<Transform, T, T1, T2, T3>> withLatestFrom(const Observable<T1>& o1, const Observable<T2>& o2, const Observable<T3>& o3, Transform&& transform) const
    {
        return impl.withLatestFrom(o1.impl, o2.impl, o3.impl, [transform](const any& v0, const any& v1, const any& v2, const any& v3) {
            return toAny(transform(v0.get<T>(), v1.get<T1>(), v2.get<T2>(), v3.get<T3>()));
        });
    }

    /** \overload */
    template<typename T1, typename T2, typename T3, typename T4, typename Transform>
    Observable<CallResult<Transform, T, T1, T2, T3, T4>> withLatestFrom(const Observable<T1>& o1, const Observable<T2>& o2, const Observable<T3>& o3, const Observable<T4>& o4, Transform&& transform) const
    {
        return impl.withLatestFrom(o1.impl, o2.impl, o3.impl, o4.impl, [transform](const any& v0, const any& v1, const any& v2, const any& v3, const any& v4) {
            return toAny(transform(v0.get<T>(), v1.get<T1>(), v2.get<T2>(), v3.get<T3>(), v4.get<T4>()));
        });
    }

    /** \overload */
    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename Transform>
    Observable<CallResult<Transform, T, T1, T2, T3, T4, T5>> withLatestFrom(const Observable<T1>& o1, const Observable<T2>& o2, const Observable<T3>& o3, const Observable<T4>& o4, const Observable<T5>& o5, Transform&& transform) const
    {
        return impl.withLatestFrom(o1.impl, o2.impl, o3.impl, o4.impl, o5.impl, [transform](const any& v0, const any& v1, const any& v2, const any& v3, const any& v4, const any& v5) {
            return toAny(transform(v0.get<T>(), v1.get<T1>(), v2.get<T2>(), v3.get<T3>(), v4.get<T4>(), v5.get<T5>()));
        });
    }

    /** \overload */
    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename Transform>
    Observable<CallResult<Transform, T, T1, T2, T3, T4, T5, T6>> withLatestFrom(const Observable<T1>& o1, const Observable<T2>& o2, const Observable<T3>& o3, const Observable<T4>& o4, const Observable<T5>& o5, const Observable<T6>& o6, Transform&& transform) const
    {
        return impl.withLatestFrom(o1.impl, o2.impl, o3.impl, o4.impl, o5.impl, o6.impl, [transform](const any& v0, const any& v1, const any& v2, const any& v3, const any& v4, const any& v5, const any& v6) {
            return toAny(transform(v0.get<T>(), v1.get<T1>(), v2.get<T2>(), v3.get<T3>(), v4.get<T4>(), v5.get<T5>(), v6.get<T6>()));
        });
    }

    /** \overload */
    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename Transform>
    Observable<CallResult<Transform, T, T1, T2, T3, T4, T5, T6, T7>> withLatestFrom(const Observable<T1>& o1, const Observable<T2>& o2, const Observable<T3>& o3, const Observable<T4>& o4, const Observable<T5>& o5, const Observable<T6>& o6, const Observable<T7>& o7, Transform&& transform) const
    {
        return impl.withLatestFrom(o1.impl, o2.impl, o3.impl, o4.impl, o5.impl, o6.impl, o7.impl, [transform](const any& v0, const any& v1, const any& v2, const any& v3, const any& v4, const any& v5, const any& v6, const any& v7) {
            return toAny(transform(v0.get<T>(), v1.get<T1>(), v2.get<T2>(), v3.get<T3>(), v4.get<T4>(), v5.get<T5>(), v6.get<T6>(), v7.get<T7>()));
        });
    }

    ///@}

    ///@{
    /**
     Returns an Observable that emits **whenever** an item is emitted by either this Observable **or** o1, o2, …. It combines the **latest** item from each Observable via the given function and emits the result of this function.
     
     It applies this function in strict sequence, so the first item emitted by the returned Observable is the result of `f` applied to the first item emitted by this Observable and the first item emitted by `o1`; the second item emitted by the returned Observable is the result of `f` applied to the second item emitted by this Observable and the second item emitted by `o1`; and so on.
     
     The returned Observable only emits as many items as the number of items emitted by the source Observable that emits the fewest items.
     */
    template<typename T1, typename Transform>
    Observable<CallResult<Transform, T, T1>> zip(const Observable<T1>& o1, Transform&& transform) const
    {
        return impl.zip(o1.impl, [transform](const any& v0, const any& v1) {
            return toAny(transform(v0.get<T>(), v1.get<T1>()));
        });
    }

    /** \overload */
    template<typename T1, typename T2, typename Transform>
    Observable<CallResult<Transform, T, T1, T2>> zip(const Observable<T1>& o1, const Observable<T2>& o2, Transform&& transform) const
    {
        return impl.zip(o1.impl, o2.impl, [transform](const any& v0, const any& v1, const any& v2) {
            return toAny(transform(v0.get<T>(), v1.get<T1>(), v2.get<T2>()));
        });
    }

    /** \overload */
    template<typename T1, typename T2, typename T3, typename Transform>
    Observable<CallResult<Transform, T, T1, T2, T3>> zip(const Observable<T1>& o1, const Observable<T2>& o2, const Observable<T3>& o3, Transform&& transform) const
    {
        return impl.zip(o1.impl, o2.impl, o3.impl, [transform](const any& v0, const any& v1, const any& v2, const any& v3) {
            return toAny(transform(v0.get<T>(), v1.get<T1>(), v2.get<T2>(), v3.get<T3>()));
        });
    }

    /** \overload */
    template<typename T1, typename T2, typename T3, typename T4, typename Transform>
    Observable<CallResult<Transform, T, T1, T2, T3, T4>> zip(const Observable<T1>& o1, const Observable<T2>& o2, const Observable<T3>& o3, const Observable<T4>& o4, Transform&& transform) const
    {
        return impl.zip(o1.impl, o2.impl, o3.impl, o4.impl, [transform](const any& v0, const any& v1, const any& v2, const any& v3, const any& v4) {
            return toAny(transform(v0.get<T>(), v1.get<T1>(), v2.get<T2>(), v3.get<T3>(), v4.get<T4>()));
        });
    }

    /** \overload */
    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename Transform>
    Observable<CallResult<Transform, T, T1, T2, T3, T4, T5>> zip(const Observable<T1>& o1, const Observable<T2>& o2, const Observable<T3>& o3, const Observable<T4>& o4, const Observable<T5>& o5, Transform&& transform) const
    {
        return impl.zip(o1.impl, o2.impl, o3.impl, o4.impl, o5.impl, [transform](const any& v0, const any& v1, const any& v2, const any& v3, const any& v4, const any& v5) {
            return toAny(transform(v0.get<T>(), v1.get<T1>(), v2.get<T2>(), v3.get<T3>(), v4.get<T4>(), v5.get<T5>()));
        });
    }

    /** \overload */
    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename Transform>
    Observable<CallResult<Transform, T, T1, T2, T3, T4, T5, T6>> zip(const Observable<T1>& o1, const Observable<T2>& o2, const Observable<T3>& o3, const Observable<T4>& o4, const Observable<T5>& o5, const Observable<T6>& o6, Transform&& transform) const
    {
        return impl.zip(o1.impl, o2.impl, o3.impl, o4.impl, o5.impl, o6.impl, [transform](const any& v0, const any& v1, const any& v2, const any& v3, const any& v4, const any& v5, const any& v6) {
            return toAny(transform(v0.get<T>(), v1.get<T1>(), v2.get<T2>(), v3.get<T3>(), v4.get<T4>(), v5.get<T5>(), v6.get<T6>()));
        });
    }

    /** \overload */
    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename Transform>
    Observable<CallResult<Transform, T, T1, T2, T3, T4, T5, T6, T7>> zip(const Observable<T1>& o1, const Observable<T2>& o2, const Observable<T3>& o3, const Observable<T4>& o4, const Observable<T5>& o5, const Observable<T6>& o6, const Observable<T7>& o7, Transform&& transform) const
    {
        return impl.zip(o1.impl, o2.impl, o3.impl, o4.impl, o5.impl, o6.impl, o7.impl, [transform](const any& v0, const any& v1, const any& v2, const any& v3, const any& v4, const any& v5, const any& v6, const any& v7) {
            return toAny(transform(v0.get<T>(), v1.get<T1>(), v2.get<T2>(), v3.get<T3>(), v4.get<T4>(), v5.get<T5>(), v6.get<T6>(), v7.get<T7>()));
        });
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
        return impl.observeOn(scheduler);
    }


#pragma mark - Misc
    /**
     Blocks until the Observable has completed, then returns an Array of all emitted items.
     
     Be careful when you use this on the message thread: If the Observable needs to process something *asynchronously* on the message thread, calling this will deadlock.
     
     ​ **If you don't pass an onError handler, an exception inside the Observable will terminate your app.**
     */
    juce::Array<T> toArray(const std::function<void(Error)>& onError = Impl::TerminateOnError) const
    {
        juce::Array<T> items;

        for (const any& v : impl.toArray(onError))
            items.add(v.get<T>());

        return items;
    }

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
        return any(std::move(u));
    }
    template<typename U>
    static typename std::enable_if<IsObservable<U>::value, any>::type toAny(U&& u)
    {
        return any(std::move(u.impl));
    }

    JUCE_LEAK_DETECTOR(Observable)
};

template<>
class Observable<void>
{
public:
    typedef detail::ObservableImpl Impl;

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
     Creates an Observable which emits a range of items, starting at `first` to (and including) `last`. It completes after emitting the `last` item.
     
     ​ **Throws an exception if first > last.**
     
     For example:
     
     Observable::range(3, 7, 3) // {3, 6, 7}
     Observable::range(17.5, 22.8, 2) // {17.5, 19.5, 21.5, 22.8}
     */
    template<typename T>
    static Observable<T> range(T first, T last, unsigned int step = 1)
    {
        static_assert(std::is_integral<T>::value, "first and last must be integral.");
        return Impl::integralRange(first, last, step);
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
