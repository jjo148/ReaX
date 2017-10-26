#pragma once

namespace detail {
template<typename T>
class LockFreeTargetBase
{
protected:
    PublishSubject<T> subject;
    DisposeBag disposeBag;
};

#if __cpp_lib_atomic_is_always_lock_free
// std::conjunction implementation, because std::conjunction may not be available
template<class...>
struct conjunction : std::true_type
{
};
template<class B1>
struct conjunction<B1> : B1
{
};
template<class B1, class... Bn>
struct conjunction<B1, Bn...>
: std::conditional_t<bool(B1::value), conjunction<Bn...>, B1>
{
};

template<typename T>
struct IsLockFree_Impl
: std::integral_constant<bool, std::atomic<T>::is_always_lock_free>
{
};

// Enable juce::Atomic for trivially copyable types that are always lock-free:
template<typename T>
using IsLockFree = typename std::enable_if<conjunction<std::is_trivially_copyable<T>, IsLockFree_Impl<T>>::value>::type;

#else
// Enable juce::Atomic for arithmetic types:
template<typename T>
using IsLockFree = typename std::enable_if<std::is_arithmetic<T>::value>::type;
#endif
}

/**
 An Observer that stores the latest retrieved item atomically, so it can be read by a different thread.
 
 In most cases, this should be used with BehaviorSubject, or Observables that immediately emit at least one item when subscribing. This ensures that getValue() isn't called before any items have been retrieved.
 
 From C++17 onwards, it uses juce::Atomic internally if T is trivially copyable and always lock-free. Before C++17, it uses juce::Atomic for arithmetic types. Otherwise, it uses std::shared_ptr and a ReleasePool to store the retrieved item.
 */
template<typename T, typename Enable = void>
class LockFreeTarget : private detail::LockFreeTargetBase<T>, public Observer<T>
{
public:
    /// Creates a new instance. You must not call getValue() before retrieving items.
    LockFreeTarget()
    : Observer<T>(detail::LockFreeTargetBase<T>::subject)
    {
        const auto onNext = [this](const T& newValue) {
            const auto latest = std::make_shared<T>(newValue);
            detail::ReleasePool::get().add(latest);
            std::atomic_store(&latestValue, latest);
        };

        detail::LockFreeTargetBase<T>::subject.subscribe(onNext).disposedBy(detail::LockFreeTargetBase<T>::disposeBag);
    }

    /// Reads the latest retrieved item atomically. This can be called from the audio thread (or some other realtime thread). **Must not be called before any item has been retrieved.**
    inline T getValue() const
    {
        const auto latest = getValuePointer();

        // Must retrieve at least one item before this is called!
        jassert(latest);

        return *latest;
    }
    
    inline std::shared_ptr<const T> getValuePointer() const
    {
        return std::atomic_load(&latestValue);
    }

    /// Returns the latest retrieved item atomically. It just calls getValue(). This can be called from the audio thread (or some other realtime thread).
    inline operator T() const { return getValue(); }

private:
    std::shared_ptr<T> latestValue;

    JUCE_LEAK_DETECTOR(LockFreeTarget)
};

/**
 An Observer that stores the latest retrieved item atomically, so it can be read by a different thread.
 
 In most cases, this should be used with BehaviorSubject, or Observables that immediately emit at least one item when subscribing. This ensures that getValue() isn't called before any items have been retrieved.
 
 From C++17 onwards, it uses juce::Atomic internally if T is trivially copyable and always lock-free. Before C++17, it uses juce::Atomic for arithmetic types. Otherwise, it uses std::shared_ptr and a ReleasePool to store the retrieved item.
 */
template<typename T>
class LockFreeTarget<T, detail::IsLockFree<T>> : private detail::LockFreeTargetBase<T>, public Observer<T>
{
public:
    /// Creates a new instance. Before retrieving items, getValue() returns an unitialized value.
    LockFreeTarget()
    : Observer<T>(detail::LockFreeTargetBase<T>::subject)
    {
        const auto onNext = [this](const T& newValue) {
            latestValue.set(newValue);
        };
        
        detail::LockFreeTargetBase<T>::subject.subscribe(onNext).disposedBy(detail::LockFreeTargetBase<T>::disposeBag);
    }

    /// Reads the latest retrieved item atomically. This can be called from the audio thread (or some other realtime thread). If this is called before any item has been retrieved, the returned value is uninitialized.
    inline T getValue() const
    {
        return latestValue.get();
    }

    /// Returns the latest retrieved item atomically. It just calls getValue().
    inline operator T() const { return getValue(); }

private:
    juce::Atomic<T> latestValue;

    JUCE_LEAK_DETECTOR(LockFreeTarget)
};
