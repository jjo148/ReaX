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
        Moves the Disposable into a given DisposeBag. The Disposable is disposed automatically when the DisposeBag is destroyed.
     
        **This moves the Disposable, so it must be used only when the Disposable is an rvalue. For example:**
     
             myObservable.subscribe([](int i){ ... }).disposedBy(myDisposeBag); // Okay
     
             Disposable myDisposable = myObservable.subscribe([](int i){ ... });
             myDisposable.disposedBy(myDisposeBag);
             myDisposable.dispose(); // NOT okay
        
        @see DisposeBag::insert()
     */
    void disposedBy(DisposeBag& disposeBag);

private:
    friend class detail::ObservableImpl;
    friend class DisposeBag;
    
    const detail::any wrapped;

    explicit Disposable(detail::any&& wrapped);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Disposable)
};
