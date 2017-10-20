#pragma once

/**
 Retrieves items. You can call onNext to notify the Observer with a new item.
 
 An Observer does **not** automatically call onCompleted when it's destroyed.
 
 @see Subject, Observable::create
 */
template<typename T>
class Observer
{
public:
    /** Notifies the Observer with a new item. */
    void onNext(const T& item) const
    {
        impl.onNext(detail::any(item));
    }

    /** Notifies the Observer that an error has occurred. */
    void onError(std::exception_ptr error) const
    {
        impl.onError(error);
    }

    /** Notifies the Observer that no more values will be pushed. */
    void onCompleted() const
    {
        impl.onCompleted();
    }
    
protected:
    Observer(const detail::ObserverImpl& impl)
    : impl(impl)
    {}

private:
#warning Maybe remove these
    friend class ObservableBase;
    template<typename U>
    friend class Observable;
    
    template<typename U>
    friend class BehaviorSubject;
    template<typename U>
    friend class PublishSubject;
    template<typename U>
    friend class ReplaySubject;
    
    const detail::ObserverImpl impl;

    JUCE_LEAK_DETECTOR(Observer)
};
