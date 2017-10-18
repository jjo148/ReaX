#pragma once

namespace detail {
    class ObserverImpl
    {
    public:
        ObserverImpl(const varx::any& wrapped);
        void onNext(const juce::var& next) const;
        void onError(Error error) const;
        void onCompleted() const;
        
    protected:
        const varx::any wrapped;
    };
}

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


/**
 Retrieves items. You can call onNext to notify the Observer with a new item.
 
 An Observer does **not** automatically call onCompleted when it's destroyed.
 
 @see Subject, Observable::create
 */
template<typename T>
class Observer : private ObserverBase
{
public:
    /** Notifies the Observer with a new item. */
    void onNext(const T& item) const
    {
        ObserverBase::onNext(toVar(item));
//        impl->onNext(toVar(item));
    }

    /** Notifies the Observer that an error has occurred. */
    void onError(const Error& error) const
    {
        ObserverBase::onError(error);
//        impl->onError(error);
    }

    /** Notifies the Observer that no more values will be pushed. */
    void onCompleted() const
    {
        ObserverBase::onCompleted();
//        impl->onCompleted();
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
    
    Observer(const std::shared_ptr<detail::ObserverImpl>& impl)
    : impl(impl)
    {}

    std::shared_ptr<detail::ObserverImpl> impl;

    JUCE_LEAK_DETECTOR(Observer)
};
