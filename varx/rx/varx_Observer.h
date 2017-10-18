#pragma once

namespace detail {
    struct ObserverImpl
    {
        ObserverImpl(varx::any&& wrapped);
        void onNext(const juce::var& next) const;
        void onError(Error error) const;
        void onCompleted() const;
        
        const varx::any wrapped;
    };
}

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
        impl.onNext(toVar(item));
    }

    /** Notifies the Observer that an error has occurred. */
    void onError(const Error& error) const
    {
        impl.onError(error);
    }

    /** Notifies the Observer that no more values will be pushed. */
    void onCompleted() const
    {
        impl.onCompleted();
    }

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
    
    Observer(detail::ObserverImpl&& impl)
    : impl(std::move(impl))
    {}

    JUCE_LEAK_DETECTOR(Observer)
};
