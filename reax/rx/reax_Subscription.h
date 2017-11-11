#pragma once

namespace detail {
    struct ObservableImpl;
}

class DisposeBag;

/**
    Manages the lifetime of a subscription to an Observable.
 
    @see Observable::subscribe
 */
class Subscription
{
public:
    /// Copy constructor.
    Subscription(const Subscription&) = default;
    
    /// Copy assignment.
    Subscription& operator=(const Subscription&) = default;
    
    /// Unsubscribes from the Observable.
    void unsubscribe() const;

    /**
        Moves the Subscription into a given DisposeBag. The Subscription is unsubscribed automatically when the DisposeBag is destroyed.
     
        Can only be used when the Subscription is an rvalue.
     
        @see DisposeBag::insert()
     */
    void disposedBy(DisposeBag& disposeBag);

private:
    friend struct detail::ObservableImpl;
    friend class DisposeBag;
    
    detail::any wrapped;

    explicit Subscription(detail::any&& wrapped);

    JUCE_LEAK_DETECTOR(Subscription)
};
