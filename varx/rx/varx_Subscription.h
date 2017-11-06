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
    ///@{
    /**
        Transfers ownership from an existing Subscription.
     */
    Subscription(Subscription&&) = default;
    Subscription& operator=(Subscription&&) = default;
    ///@}

    /**
        Unsubscribes from the Observable.
     */
    void unsubscribe() const;

    /**
        Moves the Subscription into a given DisposeBag. The Subscription is unsubscribed automatically when the DisposeBag is destroyed.
     
        Can only be used when the Subscription is an rvalue.
     
        @see DisposeBag::insert()
     */
    void disposedBy(DisposeBag& disposeBag) &&;

private:
    friend class detail::ObservableImpl;
    friend class DisposeBag;
    
    const detail::any wrapped;

    explicit Subscription(detail::any&& wrapped);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Subscription)
};
