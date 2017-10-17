#pragma once

/**
    Retrieves items. You can call onNext to notify the Observer with a new item.
 
    An Observer does **not** automatically call onCompleted when it's destroyed.
 
    @see Subject, Observable::create
 */
class Observer
{
public:
    /** Notifies the Observer with a new item. */
    template<typename T>
    void onNext(T&& next) const
    {
        _onNext(toVar(std::forward<T>(next)));
    }

    /** Notifies the Observer that an error has occurred. */
    void onError(Error error) const;

    /** Notifies the Observer that no more values will be pushed. */
    void onCompleted() const;

private:
    friend class Subject;
    friend class Observable;
    friend class ObservableBase;
    friend class SubjectBase;
    
    template<typename T>
    friend class TypedObserver;
    
    struct Impl;
    typedef std::shared_ptr<Impl> Impl_ptr;
    explicit Observer(const Impl_ptr& impl);
    Impl_ptr impl;

    void _onNext(const juce::var& next) const;

    JUCE_LEAK_DETECTOR(Observer)
};

template<typename T>
class TypedObserver : private Observer
{
public:
    void onNext(const T& item) const
    {
        Observer::onNext(toVar(item));
    }

    void onError(const Error& error) const
    {
        Observer::onError(error);
    }

    void onCompleted() const
    {
        Observer::onCompleted();
    }

private:
    friend class ObservableBase;
    
    template<typename U>
    friend class TypedObservable;
    template<typename U>
    friend class TypedBehaviorSubject;
    template<typename U>
    friend class TypedPublishSubject;
    template<typename U>
    friend class TypedReplaySubject;

    TypedObserver(const Impl_ptr& impl)
    : Observer(impl)
    {}

    JUCE_LEAK_DETECTOR(TypedObserver)
};
