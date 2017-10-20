#pragma once

namespace detail {
    class ObservableImpl;
}

class DisposeBag;

/**
    Manages the lifetime of a disposable to an Observable.
 
    @see Observable::subscribe
 */
class Disposable
{
public:
    ///@{
    /**
        Transfers ownership from an existing Disposable.
     */
    Disposable(Disposable&&) = default;
    Disposable& operator=(Disposable&&) = default;
    ///@}

    /**
        Unsubscribes from the Observable.
     */
    void dispose() const;

    /**
        Inserts the Disposable into a given DisposeBag.
        
        The Disposable is disposed when the DisposeBag is destroyed.
        
        @see DisposeBag::insert
     */
    void disposedBy(DisposeBag& disposeBag);

private:
    friend class detail::ObservableImpl;
    friend class DisposeBag;
    
    const detail::any wrapped;

    explicit Disposable(detail::any&& wrapped);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Disposable)
};
