#pragma once

class Observer;
class Scheduler;
class Subject;

/**
    An Observable is a value that changes over time.
 */
class Observable
{
public:
    //! @cond Doxygen_Suppress
    typedef juce::var var;
    //! @endcond

#pragma mark - Creation
    /**
        Creates an Observable which emits values from an Observer on each subscription.
     
        In the onSubscribe function, you get an Observer. You can call Observer::onNext on it to emit values from the Observable.
     */
    static Observable create(const std::function<void(Observer)>& onSubscribe);

    /**
        Creates a new Observable for each subscriber, by calling the `factory` function on each new subscription.
     */
    static Observable defer(const std::function<Observable()>& factory);

    /**
        Creates an Observable that doesn't emit any items and notifies onComplete immediately.
     */
    static Observable empty();

    /**
        Creates an Observable which doesn't emit any items, and immediately notifies onError.
     */
    static Observable error(const std::exception& error);

    /**
        Creates an Observable that immediately emits the items from the given Array.
     
        Note that you can also pass an initializer list, like this:
     
            Observable::from({"Hello", "Test"})
     
            Observable::from({var(3), var("four")})
     */
    static Observable from(const juce::Array<var>& array);

    /**
        Creates an Observable from a given JUCE Value. The returned Observable **only emits items until it is destroyed**, so you are responsible for managing its lifetime. Or use Reactive<Value>, which will handle this.
     
        The returned Observable notifies the onComplete handler when it's destroyed. @see Observable::subscribe
     
        When calling Value::setValue, it notifies asynchronously. So **the returned Observable emits the new value asynchronously.** If you call setValue immediately before destroying the returned Observable, the new value will not be emitted.
     */
    static Observable fromValue(juce::Value value);

    /**
        Returns an Observable that emits one item every `interval`, starting at the time of subscription (where the first item is emitted). The emitted items are `1`, `2`, `3`, and so on.
     
        The Observable emits endlessly, but you can use Observable::take to get a finite number of items (for example).
     
        The interval has millisecond resolution.
     */
    static Observable interval(const juce::RelativeTime& interval);

    /**
        Creates an Observable which emits a single item.
     
        The value is emitted immediately on each new subscription.
     */
    template<typename T>
    static Observable just(T&& value)
    {
        return _just(toVar(std::forward<T>(value)));
    }

    /**
        Creates an Observable that never emits any events and never terminates.
     */
    static Observable never();

    ///@{
    /**
        Creates an Observable which emits a range of items, starting at `first` to (and including) `last`. It completes after emitting the `last` item.
     
        ​ **Throws an exception if first > last.**
     
        For example:
     
             Observable::range(3, 7, 3) // {3, 6, 7}
             Observable::range(17.5, 22.8, 2) // {17.5, 19.5, 21.5, 22.8}
     */
    static Observable range(int first, int last, unsigned int step = 1);
    /** \overload */
    static Observable range(double first, double last, unsigned int step);
    ///@}

    /**
        Creates an Observable which emits a given item repeatedly.
     
        An optional `times` parameter specifies how often the item should be repeated. If omitted, the item will is repeated indefinitely.
     */
    template<typename T>
    static Observable repeat(T&& item)
    {
        return _repeat(toVar(std::forward<T>(item)));
    }
    /** \overload */
    template<typename T>
    static Observable repeat(T&& item, unsigned int times)
    {
        return _repeat(toVar(std::forward<T>(item)), times);
    }


#pragma mark - Disposable
    ///@{
    /**
        Subscribes to an Observable, to receive values it emits.
     
        The **onNext** function is called whenever the Observable emits a new item. It may be called synchronously before subscribe() returns.
     
        The **onError** function is called when the Observable has failed to generate the expected data, or has encountered some other error. If onError is called, the Observable will not make any more calls. **If you don't pass an onError handler, an exception inside the Observable will terminate your app.**
     
        The **onCompleted** function is called exactly once to notify that the Observable has generated all data and will not emit any more items.
     
        The returned Disposable can be used to unsubscribe from the Observable, to stop receiving values from it. **You will keep receiving values until you call Disposable::dispose, or until the Observable source is destroyed**. The best way is to use a DisposeBag, which automatically unsubscribes when it is destroyed.
     */
    Disposable subscribe(const std::function<void(const var&)>& onNext,
                         const std::function<void(Error)>& onError = TerminateOnError,
                         const std::function<void()>& onCompleted = EmptyOnCompleted) const;
    ///@}

    ///@{
    /**
        Subscribes an Observer to an Observable. The Observer is notified whenever the Observable emits an item, or notifies an onError / onCompleted.
     
        The returned Disposable can be used to unsubscribe the Observer, so it stops being notified by this Observable. **The Observer keeps receiving values until you call Disposable::dispose, or until the Observable source is destroyed**. The best way is to use a DisposeBag, which automatically unsubscribes when it is destroyed.
     */
    Disposable subscribe(const Observer& observer) const;
    ///@}


#pragma mark - Transform Functions
    ///@{
    /**
        A `FunctionN` is a function that takes `N` var arguments, and returns a var.
     */
    typedef const std::function<var(const var&)>& Function1;
    typedef const std::function<var(const var&, const var&)>& Function2;
    typedef const std::function<var(const var&, const var&, const var&)>& Function3;
    typedef const std::function<var(const var&, const var&, const var&, const var&)>& Function4;
    typedef const std::function<var(const var&, const var&, const var&, const var&, const var&)>& Function5;
    typedef const std::function<var(const var&, const var&, const var&, const var&, const var&, const var&)>& Function6;
    typedef const std::function<var(const var&, const var&, const var&, const var&, const var&, const var&, const var&)>& Function7;
    typedef const std::function<var(const var&, const var&, const var&, const var&, const var&, const var&, const var&, const var&)>& Function8;
    ///@}

    /** A `PredicateN` is a function that compares takes `N` vars and returns a bool.  */
    typedef const std::function<bool(const var&)>& Predicate1;
    typedef const std::function<bool(const var&, const var&)>& Predicate2;

#pragma mark - Operators
    ///@{
    /**
        Returns an Observable that emits **whenever** an item is emitted by either this Observable **or** o1, o2, …. It combines the **latest** item from each Observable via the given function and emits the result of this function.
     
        This is different from Observable::withLatestFrom because it emits whenever this Observable or o1, o2, … emits an item.
     
        @see Observable::withLatestFrom
     */
    Observable combineLatest(Observable o1, Function2 f = &Observable::CombineIntoArray2) const;
    /** \overload */
    Observable combineLatest(Observable o1, Observable o2, Function3 f = &Observable::CombineIntoArray3) const;
    /** \overload */
    Observable combineLatest(Observable o1, Observable o2, Observable o3, Function4 f = &Observable::CombineIntoArray4) const;
    /** \overload */
    Observable combineLatest(Observable o1, Observable o2, Observable o3, Observable o4, Function5 f = &Observable::CombineIntoArray5) const;
    /** \overload */
    Observable combineLatest(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5, Function6 f = &Observable::CombineIntoArray6) const;
    /** \overload */
    Observable combineLatest(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5, Observable o6, Function7 f = &Observable::CombineIntoArray7) const;
    /** \overload */
    Observable combineLatest(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5, Observable o6, Observable o7, Function8 f = &Observable::CombineIntoArray8) const;
    ///@}

    ///@{
    /**
        Returns an Observable that first emits the items from this Observable, then from o1, then from o2, and so on.
     
        It only subscribes to o1 when this Observable has completed. And only subscribes to o2 when o1 has completed, and so on.
     */
    Observable concat(Observable o1) const;
    /** \overload */
    Observable concat(Observable o1, Observable o2) const;
    /** \overload */
    Observable concat(Observable o1, Observable o2, Observable o3) const;
    /** \overload */
    Observable concat(Observable o1, Observable o2, Observable o3, Observable o4) const;
    /** \overload */
    Observable concat(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5) const;
    /** \overload */
    Observable concat(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5, Observable o6) const;
    /** \overload */
    Observable concat(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5, Observable o6, Observable o7) const;
    ///@}

    /**
        Returns an Observable which emits if `interval` has passed without this Observable emitting an item. The returned Observable emits the latest item from this Observable.
     
        It's like the instant search in a search engine: Search suggestions are only loaded if the user hasn't pressed a key for a short period of time.
     
        The interval has millisecond resolution.
     */
    Observable debounce(const juce::RelativeTime& interval) const;

    /**
        Returns an Observable which emits the same items as this Observable, but suppresses consecutive duplicate items.
     
        If you just use distinctUntilChanged() without a template parameter, it uses juce::var::operator== to determine whether two items are equal.
     
        **If you are using VARX_DEFINE_VARIANT_CONVERTER to wrap a custom type T into a var, you should call distinctUntilChanged<T>(), declaring your custom type T.** This way, it uses T::operator== to determine whether two items are equal.
     
        If, for some reason, the custom type T doesn't have operator==, you can pass a custom equality function.
     */
    template<typename T = juce::var>
    Observable distinctUntilChanged(const std::function<bool(const T&, const T&)>& equals = std::equal_to<T>()) const
    {
        return _distinctUntilChanged([equals](const var& v1, const var& v2) {
            return equals(fromVar<T>(v1), fromVar<T>(v2));
        });
    }

    /**
        Returns an Observable which emits only one item: The `index`th item emitted by this Observable.
     */
    Observable elementAt(int index) const;

    /**
        Returns an Observable that emits only those items from this Observable that pass a predicate function.
     */
    Observable filter(Predicate1 predicate) const;

    /**
        For each emitted item, calls `f` and subscribes to the Observable returned from `f`. The emitted items from all these returned Observables are *merged* (so they interleave).
     
        This Observable:
     
            Observable::from({"Hello", "World"}).flatMap([](String s) {
                return Observable::from({s.toLowerCase(), s.toUpperCase() + "!"});
            });
     
        Will emit the items: `"hello"`, `"HELLO!"`, `"world"` and `"WORLD!"`.
     
        @see Observable::merge, Observable::switchOnNext.
     */
    Observable flatMap(const std::function<Observable(const var&)>& f) const;

    /**
        For each item emitted by this Observable, call the function with that item and emit the result.
     
        If `f` returns an Observable, you can use Observable::switchOnNext afterwards.
     */
    Observable map(Function1 f) const;

    ///@{
    /**
        Merges the emitted items of this observable and o1, o2, … into one Observable. The items are interleaved, depending on when the source Observables emit items.

        An error in one of the source Observables notifies the result Observable's `onError` immediately.
     */
    Observable merge(Observable o1) const;
    /** \overload */
    Observable merge(Observable o1, Observable o2) const;
    /** \overload */
    Observable merge(Observable o1, Observable o2, Observable o3) const;
    /** \overload */
    Observable merge(Observable o1, Observable o2, Observable o3, Observable o4) const;
    /** \overload */
    Observable merge(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5) const;
    /** \overload */
    Observable merge(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5, Observable o6) const;
    /** \overload */
    Observable merge(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5, Observable o6, Observable o7) const;
    ///@}

    /**
        Begins with a `startValue`, and then applies `f` to all items emitted by this Observable, and returns the aggregate result as a single-element Observable sequence.
     */
    Observable reduce(const var& startValue, Function2 f) const;

    /**
        Returns an Observable which checks every `interval` milliseconds whether this Observable has emitted any new items. If so, the returned Observable emits the latest item from this Observable.
     
        For example, this is useful when an Observable emits items very rapidly, but you only want to update a GUI component 25 times per second to reduce CPU load.
     
        The interval has millisecond resolution.
     */
    Observable sample(const juce::RelativeTime& interval);

    /**
        Calls a function `f` with the given `startValue` and the first item emitted by this Observable. The value returned from `f` is remembered. When the second item is emitted, `f` is called with the remembered value (called the *accumulator*) and the second emitted item. The returned item is remembered, until the third item is emitted, and so on.
     
        The first parameter to `f` is the accumulator, the second is the current item.
     */
    Observable scan(const var& startValue, Function2 f) const;

    /**
        Returns an Observable which suppresses emitting the first `numItems` items from this Observable.
     */
    Observable skip(unsigned int numItems) const;

    /**
        Returns an Observable which suppresses emitting items from this Observable until the `other` Observable sequence emits an item.
     */
    Observable skipUntil(Observable other) const;

    /**
        Emits the given item(s) before beginning to emit the items in this Observable.
     */
    Observable startWith(const var& item1) const;
    /** \overload */
    Observable startWith(const var& item1, const var& item2) const;
    /** \overload */
    Observable startWith(const var& item1, const var& item2, const var& item3) const;
    /** \overload */
    Observable startWith(const var& item1, const var& item2, const var& item3, const var& item4) const;
    /** \overload */
    Observable startWith(const var& item1, const var& item2, const var& item3, const var& item4, const var& item5) const;
    /** \overload */
    Observable startWith(const var& item1, const var& item2, const var& item3, const var& item4, const var& item5, const var& item6) const;
    /** \overload */
    Observable startWith(const var& item1, const var& item2, const var& item3, const var& item4, const var& item5, const var& item6, const var& item7) const;
    /** \overload */
    Observable startWith(const var& item1, const var& item2, const var& item3, const var& item4, const var& item5, const var& item6, const var& item7, const var& item8) const;

    /**
        ​ **This must only be called if this Observable emits Observables**.
     
        Returns an Observable that emits the items emitted by the Observables which this Observable emits.
     */
    Observable switchOnNext() const;

    /**
        Returns an Observable that emits only the first `numItems` items from this Observable.
     */
    Observable take(unsigned int numItems) const;

    /**
        Returns an Observable that emits only the last `numItems` items from this Observable.
     */
    Observable takeLast(unsigned int numItems) const;

    /**
        Emits items from this Observable until the `other` Observable sequence emits an item.
     */
    Observable takeUntil(Observable other) const;

    /**
        Emits items from the beginning of this Observable as long as the given `predicate` returns `true`.
     
        The predicate is called on each item emitted by this Observable, until it returns `false`.
     */
    Observable takeWhile(Predicate1 predicate) const;

    ///@{
    /**
        Returns an Observable that emits whenever an item is emitted by this Observable. It combines the latest item from each Observable via the given function and emits the result of this function.
     
        This is different from Observable::combineLatest because it only emits when this Observable emits an item (not when o1, o2, … emit items).
     */
    Observable withLatestFrom(Observable o1, Function2 f = &Observable::CombineIntoArray2) const;
    /** \overload */
    Observable withLatestFrom(Observable o1, Observable o2, Function3 f = &Observable::CombineIntoArray3) const;
    /** \overload */
    Observable withLatestFrom(Observable o1, Observable o2, Observable o3, Function4 f = &Observable::CombineIntoArray4) const;
    /** \overload */
    Observable withLatestFrom(Observable o1, Observable o2, Observable o3, Observable o4, Function5 f = &Observable::CombineIntoArray5) const;
    /** \overload */
    Observable withLatestFrom(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5, Function6 f = &Observable::CombineIntoArray6) const;
    /** \overload */
    Observable withLatestFrom(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5, Observable o6, Function7 f = &Observable::CombineIntoArray7) const;
    /** \overload */
    Observable withLatestFrom(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5, Observable o6, Observable o7, Function8 f = &Observable::CombineIntoArray8) const;
    ///@}

    ///@{
    /**
        Returns an Observable that emits **whenever** an item is emitted by either this Observable **or** o1, o2, …. It combines the **latest** item from each Observable via the given function and emits the result of this function.
     
        It applies this function in strict sequence, so the first item emitted by the returned Observable is the result of `f` applied to the first item emitted by this Observable and the first item emitted by `o1`; the second item emitted by the returned Observable is the result of `f` applied to the second item emitted by this Observable and the second item emitted by `o1`; and so on.
     
        The returned Observable only emits as many items as the number of items emitted by the source Observable that emits the fewest items.
     */
    Observable zip(Observable o1, Function2 f = &Observable::CombineIntoArray2) const;
    /** \overload */
    Observable zip(Observable o1, Observable o2, Function3 f = &Observable::CombineIntoArray3) const;
    /** \overload */
    Observable zip(Observable o1, Observable o2, Observable o3, Function4 f = &Observable::CombineIntoArray4) const;
    /** \overload */
    Observable zip(Observable o1, Observable o2, Observable o3, Observable o4, Function5 f = &Observable::CombineIntoArray5) const;
    /** \overload */
    Observable zip(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5, Function6 f = &Observable::CombineIntoArray6) const;
    /** \overload */
    Observable zip(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5, Observable o6, Function7 f = &Observable::CombineIntoArray7) const;
    /** \overload */
    Observable zip(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5, Observable o6, Observable o7, Function8 f = &Observable::CombineIntoArray8) const;
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
    Observable observeOn(const Scheduler& scheduler) const;


#pragma mark - Misc
    /**
        Wraps the Observable into a var.
     
        This allows you to return an Observable from a function, e.g. when using Observable::map.
     */
    operator juce::var() const;

    /**
        Blocks until the Observable has completed, then returns an Array of all emitted items.
     
        Be careful when you use this on the message thread: If the Observable needs to process something *asynchronously* on the message thread, calling this will deadlock.
     
        ​ **If you don't pass an onError handler, an exception inside the Observable will terminate your app.**
     */
    juce::Array<var> toArray(const std::function<void(Error)>& onError = TerminateOnError) const;


private:
    friend class Subject;
    struct Impl;
    Observable(const std::shared_ptr<Impl>&);
    std::shared_ptr<Impl> impl;

    static var CombineIntoArray2(const var&, const var&);
    static var CombineIntoArray3(const var&, const var&, const var&);
    static var CombineIntoArray4(const var&, const var&, const var&, const var&);
    static var CombineIntoArray5(const var&, const var&, const var&, const var&, const var&);
    static var CombineIntoArray6(const var&, const var&, const var&, const var&, const var&, const var&);
    static var CombineIntoArray7(const var&, const var&, const var&, const var&, const var&, const var&, const var&);
    static var CombineIntoArray8(const var&, const var&, const var&, const var&, const var&, const var&, const var&, const var&);

    static std::shared_ptr<Impl> _just(const var& value);
    static Observable _repeat(const var& item);
    static Observable _repeat(const var& item, unsigned int times);
    Observable _distinctUntilChanged(Predicate2 equals) const;

    static const std::function<void(Error)> TerminateOnError;
    static const std::function<void()> EmptyOnCompleted;
    JUCE_LEAK_DETECTOR(Observable)
};



class ObservableBase
{
protected:
    friend class SubjectBase;
    template<typename T>
    friend class TypedObservable;
    
    using var = juce::var;
    
    struct Impl;
    typedef std::shared_ptr<Impl> Impl_ptr;
    Impl_ptr impl;
    
    ObservableBase(const Impl_ptr&);
    
    // Creation
    static Impl_ptr create(const std::function<void(const Observer&)>& onSubscribe);
    static Impl_ptr defer(const std::function<ObservableBase()>& factory);
    static Impl_ptr empty();
    static Impl_ptr error(const std::exception& error);
    static Impl_ptr from(juce::Array<var>&& items);
    static Impl_ptr fromValue(juce::Value value);
    static Impl_ptr interval(const juce::RelativeTime& interval);
    static Impl_ptr just(const var& value);
    static Impl_ptr never();
    static Impl_ptr integralRange(long long first, long long last, unsigned int step);
    static Impl_ptr floatRange(float first, float last, unsigned int step);
    static Impl_ptr doubleRange(double first, double last, unsigned int step);
    static Impl_ptr repeat(const var& item);
    static Impl_ptr repeat(const var& item, unsigned int times);
    
    // Subscription
    Disposable subscribe(const std::function<void(const var&)>& onNext,
                         const std::function<void(Error)>& onError,
                         const std::function<void()>& onCompleted) const;
    Disposable subscribe(const Observer& observer) const;
    
    // Operators
    Impl_ptr combineLatest(const ObservableBase& o1, const std::function<var(const var&, const var&)>& transform) const;
    Impl_ptr concat(const ObservableBase& o1) const;
    Impl_ptr debounce(const juce::RelativeTime& interval) const;
    Impl_ptr distinctUntilChanged(const std::function<bool(const var&, const var&)>& equals) const;
    Impl_ptr elementAt(int index) const;
    Impl_ptr filter(const std::function<bool(const var&)>& predicate) const;
    Impl_ptr flatMap(const std::function<ObservableBase(const var&)>& transform) const;
    Impl_ptr map(const std::function<var(const var&)>& transform) const;
    Impl_ptr merge(const ObservableBase& o1) const;
    Impl_ptr reduce(const var& startValue, const std::function<var(const var&, const var&)>& f) const;
    Impl_ptr sample(const juce::RelativeTime& interval) const;
    Impl_ptr scan(const var& startValue, const std::function<var(const var&, const var&)>& f) const;
    Impl_ptr skip(unsigned int numItems) const;
    Impl_ptr skipUntil(const ObservableBase& other) const;
    Impl_ptr startWith(const var& item) const;
    Impl_ptr switchOnNext() const;
    Impl_ptr take(unsigned int numItems) const;
    Impl_ptr takeLast(unsigned int numItems) const;
    Impl_ptr takeUntil(const ObservableBase& other) const;
    Impl_ptr takeWhile(const std::function<bool(const var&)>& predicate) const;
    Impl_ptr withLatestFrom(const ObservableBase& o1, const std::function<var(const var&, const var&)>& transform) const;
    Impl_ptr zip(const ObservableBase& o1, const std::function<var(const var&, const var&)>& transform) const;
    
    // Scheduling
    Impl_ptr observeOn(const Scheduler& scheduler) const;
    
    // Misc
    juce::Array<var> toArray(const std::function<void(Error)>& onError) const;
    
    static void TerminateOnError(const Error&);
    static void EmptyOnCompleted();
};



template<typename T>
class TypedObserver;


template<typename T = void>
class TypedObservable : public ObservableBase
{
public:
    typedef T ItemType;
    
    template<typename U>
    struct IsObservable : std::false_type {};
    
    template<typename U>
    struct IsObservable<TypedObservable<U>> : std::true_type {};

#pragma mark - Creation
    /**
     Creates an Observable which emits values from an Observer on each subscription.
     
     In the onSubscribe function, you get an Observer. You can call Observer::onNext on it to emit values from the Observable.
     */
    static TypedObservable<T> create(const std::function<void(const TypedObserver<T>&)>& onSubscribe)
    {
        return ObservableBase::create(onSubscribe);
    }

    /**
     Creates a new Observable for each subscriber, by calling the `factory` function on each new subscription.
     */
    static TypedObservable<T> defer(const std::function<TypedObservable<T>()>& factory)
    {
        return ObservableBase::defer(factory);
    }

    /**
     Creates an Observable that doesn't emit any items and notifies onComplete immediately.
     */
    static TypedObservable<T> empty()
    {
        return ObservableBase::empty();
    }

    /**
     Creates an Observable which doesn't emit any items, and immediately notifies onError.
     */
    static TypedObservable<T> error(const std::exception& error)
    {
        return ObservableBase::error(error);
    }

    /**
     Creates an Observable that immediately emits the items from the given Array.
     
     Note that you can also pass an initializer list, like this:
     
     Observable::from({"Hello", "Test"})
     
     Observable::from({var(3), var("four")})
     */
    static TypedObservable<T> from(const juce::Array<T>& array)
    {
        juce::Array<var> items;
        
        for (auto& item : array)
            items.add(toVar(item));
        
        return ObservableBase::from(std::move(items));
    }

    /**
     Creates an Observable which emits a single item.
     
     The value is emitted immediately on each new subscription.
     */
    static TypedObservable<T> just(const T& value)
    {
        return ObservableBase::just(toVar(value));
    }

    /**
     Creates an Observable that never emits any events and never terminates.
     */
    static TypedObservable<T> never()
    {
        return ObservableBase::never();
    }

    /**
     Creates an Observable which emits a given item repeatedly.
     
     An optional `times` parameter specifies how often the item should be repeated. If omitted, the item will is repeated indefinitely.
     */
    static TypedObservable<T> repeat(const T& item)
    {
        return ObservableBase::repeat(toVar(item));
    }
    /** \overload */
    static TypedObservable<T> repeat(const T& item, unsigned int times)
    {
        return ObservableBase::repeat(toVar(item), times);
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
                         const std::function<void(Error)>& onError = TerminateOnError,
                         const std::function<void()>& onCompleted = EmptyOnCompleted) const
    {
        return ObservableBase::subscribe([onNext](const var& next) {
            onNext(fromVar<T>(next));
        }, onError, onCompleted);
    }
    ///@}

    ///@{
    /**
     Subscribes an Observer to an Observable. The Observer is notified whenever the Observable emits an item, or notifies an onError / onCompleted.
     
     The returned Disposable can be used to unsubscribe the Observer, so it stops being notified by this Observable. **The Observer keeps receiving values until you call Disposable::dispose, or until the Observable source is destroyed**. The best way is to use a DisposeBag, which automatically unsubscribes when it is destroyed.
     */
    Disposable subscribe(const TypedObserver<T>& observer) const
    {
        return ObservableBase::subscribe(observer);
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
    template<typename T1, typename Transform>
    TypedObservable<CallResult<Transform, T, T1>> combineLatest(const TypedObservable<T1>& o1, Transform&& transform) const
    {
        return ObservableBase::combineLatest(o1, [transform](const var& v0, const var& v1) {
            return toVar(transform(fromVar<T>(v0), fromVar<T1>(v1)));
        });
    }
    ///@}

    ///@{
    /**
     Returns an Observable that first emits the items from this Observable, then from o1, then from o2, and so on.
     
     It only subscribes to o1 when this Observable has completed. And only subscribes to o2 when o1 has completed, and so on.
     */
    TypedObservable<T> concat(const TypedObservable<T>& o1) const
    {
        return ObservableBase::concat(o1);
    }
    ///@}

    /**
     Returns an Observable which emits if `interval` has passed without this Observable emitting an item. The returned Observable emits the latest item from this Observable.
     
     It's like the instant search in a search engine: Search suggestions are only loaded if the user hasn't pressed a key for a short period of time.
     
     The interval has millisecond resolution.
     */
    TypedObservable<T> debounce(const juce::RelativeTime& interval) const
    {
        return ObservableBase::debounce(interval);
    }

    /**
     Returns an Observable which emits the same items as this Observable, but suppresses consecutive duplicate items.
     
     If you just use distinctUntilChanged() without a template parameter, it uses juce::var::operator== to determine whether two items are equal.
     
     **If you are using VARX_DEFINE_VARIANT_CONVERTER to wrap a custom type T into a var, you should call distinctUntilChanged<T>(), declaring your custom type T.** This way, it uses T::operator== to determine whether two items are equal.
     
     If, for some reason, the custom type T doesn't have operator==, you can pass a custom equality function.
     */
    TypedObservable<T> distinctUntilChanged(const std::function<bool(const T&, const T&)>& equals = std::equal_to<T>()) const
    {
        return ObservableBase::distinctUntilChanged([equals](const var& lhs, const var& rhs) {
            return equals(fromVar<T>(lhs), fromVar<T>(rhs));
        });
    }

    /**
     Returns an Observable which emits only one item: The `index`th item emitted by this Observable.
     */
    TypedObservable<T> elementAt(int index) const
    {
        return ObservableBase::elementAt(index);
    }

    /**
     Returns an Observable that emits only those items from this Observable that pass a predicate function.
     */
    TypedObservable<T> filter(const std::function<bool(const T&)>& predicate) const
    {
        return ObservableBase::filter([predicate](const var& item) {
            return predicate(fromVar<T>(item));
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
    typename std::enable_if<IsObservable<CallResult<Transform, T>>::value, TypedObservable<typename CallResult<Transform, T>::ItemType>>::type flatMap(Transform&& transform) const
    {
        return ObservableBase::flatMap([transform](const var& item) {
            return transform(fromVar<T>(item));
        });
    }

    /**
     For each item emitted by this Observable, call the function with that item and emit the result.
     
     If `f` returns an Observable, you can use Observable::switchOnNext afterwards.
     */
    template<typename Transform>
    TypedObservable<CallResult<Transform, T>> map(Transform&& transform) const
    {
        return ObservableBase::map([transform](const var& item) {
            return toVar(transform(fromVar<T>(item)));
        });
    }

    ///@{
    /**
     Merges the emitted items of this observable and o1, o2, … into one Observable. The items are interleaved, depending on when the source Observables emit items.
     
     An error in one of the source Observables notifies the result Observable's `onError` immediately.
     */
    TypedObservable<T> merge(const TypedObservable<T>& o1) const
    {
        return ObservableBase::merge(o1);
    }
    
    ///@}

    /**
     Begins with a `startValue`, and then applies `f` to all items emitted by this Observable, and returns the aggregate result as a single-element Observable sequence.
     */
    TypedObservable<T> reduce(const T& startValue, const std::function<T(const T&, const T&)>& f) const
    {
        return ObservableBase::reduce(toVar(startValue), [f](const var& v1, const var& v2) {
            return toVar(f(fromVar<T>(v1), fromVar<T>(v2)));
        });
    }

    /**
     Returns an Observable which checks every `interval` milliseconds whether this Observable has emitted any new items. If so, the returned Observable emits the latest item from this Observable.
     
     For example, this is useful when an Observable emits items very rapidly, but you only want to update a GUI component 25 times per second to reduce CPU load.
     
     The interval has millisecond resolution.
     */
    TypedObservable<T> sample(const juce::RelativeTime& interval) const
    {
        return ObservableBase::sample(interval);
    }

    /**
     Calls a function `f` with the given `startValue` and the first item emitted by this Observable. The value returned from `f` is remembered. When the second item is emitted, `f` is called with the remembered value (called the *accumulator*) and the second emitted item. The returned item is remembered, until the third item is emitted, and so on.
     
     The first parameter to `f` is the accumulator, the second is the current item.
     */
    TypedObservable<T> scan(const T& startValue, const std::function<T(const T&, const T&)>& f) const
    {
        return ObservableBase::scan(toVar(startValue), [f](const var& v1, const var& v2) {
            return toVar(f(fromVar<T>(v1), fromVar<T>(v2)));
        });
    }

    /**
     Returns an Observable which suppresses emitting the first `numItems` items from this Observable.
     */
    TypedObservable<T> skip(unsigned int numItems) const
    {
        return ObservableBase::skip(numItems);
    }

    /**
     Returns an Observable which suppresses emitting items from this Observable until the `other` Observable sequence emits an item.
     */
    template<typename T1>
    TypedObservable<T> skipUntil(const TypedObservable<T1>& other) const
    {
        return ObservableBase::skipUntil(other);
    }

    /**
     Emits the given item(s) before beginning to emit the items in this Observable.
     */
    TypedObservable<T> startWith(const T& item) const
    {
        return ObservableBase::startWith(toVar(item));
    }

    /**
     ​ **This must only be called if this Observable emits Observables**.
     
     Returns an Observable that emits the items emitted by the Observables which this Observable emits.
     */
    template<typename U = T>
    typename std::enable_if<IsObservable<U>::value, U>::type switchOnNext() const
    {
        return ObservableBase::switchOnNext();
    }
    
    /**
     Returns an Observable that emits only the first `numItems` items from this Observable.
     */
    TypedObservable<T> take(unsigned int numItems) const
    {
        return ObservableBase::take(numItems);
    }
    
    /**
     Returns an Observable that emits only the last `numItems` items from this Observable.
     */
    TypedObservable<T> takeLast(unsigned int numItems) const
    {
        return ObservableBase::takeLast(numItems);
    }
    
    /**
     Emits items from this Observable until the `other` Observable sequence emits an item.
     */
    template<typename U>
    TypedObservable<T> takeUntil(const TypedObservable<U>& other) const
    {
        return ObservableBase::takeUntil(other);
    }

    /**
     Emits items from the beginning of this Observable as long as the given `predicate` returns `true`.
     
     The predicate is called on each item emitted by this Observable, until it returns `false`.
     */
    TypedObservable<T> takeWhile(const std::function<bool(const T&)>& predicate) const
    {
        return ObservableBase::takeWhile([predicate](const var& item) {
            return predicate(fromVar<T>(item));
        });
    }
    
    ///@{
    /**
     Returns an Observable that emits whenever an item is emitted by this Observable. It combines the latest item from each Observable via the given function and emits the result of this function.
     
     This is different from Observable::combineLatest because it only emits when this Observable emits an item (not when o1, o2, … emit items).
     */
    template<typename T1, typename Transform>
    TypedObservable<CallResult<Transform, T, T1>> withLatestFrom(const TypedObservable<T1>& o1, Transform&& transform) const
    {
        return ObservableBase::withLatestFrom(o1, [transform](const var& v0, const var& v1) {
            return toVar(transform(fromVar<T>(v0), fromVar<T1>(v1)));
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
    TypedObservable<CallResult<Transform, T, T1>> zip(const TypedObservable<T1>& o1, Transform&& transform) const
    {
        return ObservableBase::zip(o1, [transform](const var& v0, const var& v1) {
            return toVar(transform(fromVar<T>(v0), fromVar<T1>(v1)));
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
    TypedObservable<T> observeOn(const Scheduler& scheduler) const
    {
        return ObservableBase::observeOn(scheduler);
    }
    

#pragma mark - Misc
    /**
     Blocks until the Observable has completed, then returns an Array of all emitted items.
     
     Be careful when you use this on the message thread: If the Observable needs to process something *asynchronously* on the message thread, calling this will deadlock.
     
     ​ **If you don't pass an onError handler, an exception inside the Observable will terminate your app.**
     */
    juce::Array<T> toArray(const std::function<void(Error)>& onError = TerminateOnError) const
    {
        juce::Array<T> items;
        
        for(var& v : ObservableBase::toArray(onError))
            items.add(fromVar<T>(v));
        
        return items;
    }
    
private:
    template<typename U>
    friend class TypedObservable;
    
    TypedObservable(const Impl_ptr& impl)
    : ObservableBase(impl)
    {}
    
    JUCE_LEAK_DETECTOR(TypedObservable)
};

template<>
class TypedObservable<void>
{
public:
    /**
     Creates an Observable from a given JUCE Value. The returned Observable **only emits items until it is destroyed**, so you are responsible for managing its lifetime. Or use Reactive<Value>, which will handle this.
     
     The returned Observable notifies the onComplete handler when it's destroyed. @see Observable::subscribe
     
     When calling Value::setValue, it notifies asynchronously. So **the returned Observable emits the new value asynchronously.** If you call setValue immediately before destroying the returned Observable, the new value will not be emitted.
     */
    static TypedObservable<juce::var> fromValue(juce::Value value)
    {
        return ObservableBase::fromValue(value);
    }
    
    /**
     Returns an Observable that emits one item every `interval`, starting at the time of subscription (where the first item is emitted). The emitted items are `1`, `2`, `3`, and so on.
     
     The Observable emits endlessly, but you can use Observable::take to get a finite number of items (for example).
     
     The interval has millisecond resolution.
     */
    static TypedObservable<int> interval(const juce::RelativeTime& interval)
    {
        return ObservableBase::interval(interval);
    }
    
    /**
     Creates an Observable which emits a range of items, starting at `first` to (and including) `last`. It completes after emitting the `last` item.
     
     ​ **Throws an exception if first > last.**
     
     For example:
     
     Observable::range(3, 7, 3) // {3, 6, 7}
     Observable::range(17.5, 22.8, 2) // {17.5, 19.5, 21.5, 22.8}
     */
    template<typename T>
    static TypedObservable<T> range(T first, T last, unsigned int step = 1)
    {
        static_assert(std::is_integral<T>::value, "first and last must be integral.");
        return ObservableBase::integralRange(first, last, step);
    }
};


template<>
inline TypedObservable<float> TypedObservable<void>::range(float first, float last, unsigned int step)
{
    return ObservableBase::floatRange(first, last, step);
}

template<>
inline TypedObservable<double> TypedObservable<void>::range(double first, double last, unsigned int step)
{
    return ObservableBase::doubleRange(first, last, step);
}
