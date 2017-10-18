#pragma once

/**
    Retrieves items. You can call onNext to notify the Observer with a new item.
 
    An Observer does **not** automatically call onCompleted when it's destroyed.
 
    @see Subject, Observable::create
 */
class ObserverBase
{
public:
    /** Notifies the Observer with a new item. */
    void onNext(const juce::var& next) const;

    /** Notifies the Observer that an error has occurred. */
    void onError(Error error) const;

    /** Notifies the Observer that no more values will be pushed. */
    void onCompleted() const;

private:
    friend class Subject;
    friend class ObservableBase;
    friend class SubjectBase;
    
    template<typename T>
    friend class Observer;
    
    template<typename T>
    friend class Observable;
    
    struct Impl;
    typedef std::shared_ptr<Impl> Impl_ptr;
    explicit ObserverBase(const Impl_ptr& impl);
    Impl_ptr impl;
};

template<typename T>
class Observer : private ObserverBase
{
public:
    void onNext(const T& item) const
    {
        ObserverBase::onNext(toVar(item));
    }

    void onError(const Error& error) const
    {
        ObserverBase::onError(error);
    }

    void onCompleted() const
    {
        ObserverBase::onCompleted();
    }

private:
    friend class ObservableBase;
    
    template<typename U>
    friend class Observable;
    template<typename U>
    friend class BehaviorSubject;
    template<typename U>
    friend class PublishSubject;
    template<typename U>
    friend class ReplaySubject;

    Observer(const Impl_ptr& impl)
    : ObserverBase(impl)
    {}

    JUCE_LEAK_DETECTOR(Observer)
};
