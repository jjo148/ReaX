#pragma once

namespace detail {
    template<typename T>
    class LockFreeTargetBase
    {
    protected:
        PublishSubject<T> subject;
        DisposeBag disposeBag;
    };
}

/**
 An Observer that stores the latest retrieved item atomically, so it can be read by a different thread.
 
 In most cases, this should be used with BehaviorSubject, or Observables that immediately emit at least one item when subscribing. This ensures that getValue() isn't called before any items have been retrieved.
 
 Uses std::atomic for arithmetic types (i.e. bool and numbers). Otherwise, it uses shared_ptr and a ReleasePool to store the retrieved item.
 */
template<typename T, bool = std::is_arithmetic<T>::value>
class LockFreeTarget : private detail::LockFreeTargetBase<T>, public Observer<T>
{
public:
    /** Creates a new instance. Before retrieving items, getValue() returns an unitialized value. */
    LockFreeTarget()
    : Observer<T>(detail::LockFreeTargetBase<T>::subject)
    {
        detail::LockFreeTargetBase<T>::subject.subscribe([this](const T& newValue) {
            latestValue.store(newValue);
        }).disposedBy(detail::LockFreeTargetBase<T>::disposeBag);
    }
    
    /** Reads the latest retrieved item atomically. This can be called from the audio thread (or some other realtime thread). If this is called before any item has been retrieved, the returned value is uninitialized. */
    inline T getValue() const
    {
        return latestValue;
    }
    
    /** Returns the latest retrieved item atomically. It just calls getValue(). */
    inline operator T() const { return getValue(); }
    
private:
    std::atomic<T> latestValue;
    
    JUCE_LEAK_DETECTOR(LockFreeTarget)
};

/**
 An Observer that stores the latest retrieved item atomically, so it can be read by a different thread.
 
 In most cases, this should be used with BehaviorSubject, or Observables that immediately emit at least one item when subscribing. This ensures that getValue() isn't called before any items have been retrieved.
 
 Uses std::atomic for arithmetic types (i.e. bool and numbers). Otherwise, it uses shared_ptr and a ReleasePool to store the retrieved item.
 */
template<typename T>
class LockFreeTarget<T, false> : private detail::LockFreeTargetBase<T>, public Observer<T>
{
public:
    /** Creates a new instance. You must not call getValue() before retrieving items. */
    LockFreeTarget()
    : Observer<T>(detail::LockFreeTargetBase<T>::subject)
    {
        detail::LockFreeTargetBase<T>::subject.subscribe([this](const T& newValue) {
            const auto latest = std::make_shared<T>(newValue);
            detail::ReleasePool::get().add(latest);
            std::atomic_store(&latestValue, latest);
        }).disposedBy(detail::LockFreeTargetBase<T>::disposeBag);
    }
    
    /** Reads the latest retrieved item atomically. This can be called from the audio thread (or some other realtime thread). **Must not be called before any item has been retrieved.** */
    inline T getValue() const
    {
        const auto latest = std::atomic_load(&latestValue);
        
        // Must retrieve at least one item before this is called!
        jassert(latest);
        
        return *latest;
    }
    
    /** Returns the latest retrieved item atomically. It just calls getValue(). This can be called from the audio thread (or some other realtime thread). */
    inline operator T() const { return getValue(); }
    
private:
    std::shared_ptr<T> latestValue;
    
    JUCE_LEAK_DETECTOR(LockFreeTarget)
};
