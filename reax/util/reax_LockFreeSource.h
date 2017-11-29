#pragma once

namespace detail {
template<typename T>
class LockFreeSourceBase
{
protected:
    PublishSubject<T> subject;
};
}

/**
 Determines what should be done if the queue is full. This happens when onNext() is called too often in a row, without the message thread taking values from the queue in between.
 
 Allocate: Allocate dynamic memory to make room for more values. You will most likely call onNext() on the realtime thread, so only use this if you cannot drop any values, and make sure to pick a sufficiently large queueCapacity.
 
 DropNewest: Never allocate memory. If the queue is full, onNext() does nothing, and the new value is discarded.
 
 DropOldest: Never allocate memory. If the queue is full, the oldest value is removed to make room for a new value. If you only ever need the latest state, you can use this policy with a queueCapacity of 1.
 */
enum class CongestionPolicy {
    Allocate,
    DropNewest,
    DropOldest
};

/**
 An Observable that receives values from a realtime thread (like the audio thread) and emits those values on the JUCE message thread.
 
 The value type must be copy-constructible or (preferably) move-constructible.
 
 Call asObservable() to get the Observable, subscribe to it, etc. Then call LockFreeSource::onNext on the realtime thread to emit values.
 */
template<typename T>
class LockFreeSource : private detail::LockFreeSourceBase<T>, private juce::AsyncUpdater, public Observable<T>
{
public:
    /**
     Creates a new instance.
     
     The queueCapacity must be > 0. If you have to use CongestionPolicy::Allocate, use a large capacity, to make dynamic allocation on the audio thread as unlikely as possible. **The given `queueCapacity` may get rounded up to a different value.**
     */
    explicit LockFreeSource(size_t queueCapacity, const T& dummy = T())
    : Observable<T>(detail::LockFreeSourceBase<T>::subject),
      queue(queueCapacity),
      dummy(dummy)
    {
        // The queue capacity must be > 0.
        jassert(queueCapacity > 0);
    }

    ///@{
    /**
     Adds a value that will be emitted from the Observable.
     
     The congestionPolicy determines what to do if the queue is full. @see CongestionPolicy
     */
    void onNext(const T& value, CongestionPolicy congestionPolicy)
    {
        _onNext(value, congestionPolicy);
    }

    void onNext(T&& value, CongestionPolicy congestionPolicy)
    {
        _onNext(std::move(value), congestionPolicy);
    }
    ///@}

private:
    moodycamel::ConcurrentQueue<T> queue;
    const T dummy;

    template<typename U>
    void _onNext(U&& value, CongestionPolicy congestionPolicy)
    {
        bool needsUpdate = false;
        
        switch (congestionPolicy) {
            // If allocation is allowed, just enqueue the value, allowing the queue to allocate memory if needed.
            case CongestionPolicy::Allocate:
                queue.enqueue(std::forward<U>(value));
                needsUpdate = true;
                break;

            // If the newest value(s) may be dropped, just try to enqueue (without allocating), and do nothing if it fails.
            case CongestionPolicy::DropNewest:
                needsUpdate = queue.try_enqueue(std::forward<U>(value));
                break;

            // If the oldest value may be dropped, try to enqueue (without allocating), and remove the oldest value if needed.
            case CongestionPolicy::DropOldest: {
                // Try to enqueue the value. If it succeeds, there's no need to copy the dummy.
                // Cannot use std::forward here: Value must not be moved because try_enqueue may be called again (multiple times) below.
                if (queue.try_enqueue(value)) {
                    needsUpdate = true;
                    break;
                }
                
                // Queue is full. Drop values from the front until there's space again:
                T unused(dummy);
                while (!queue.try_enqueue(value))
                    queue.try_dequeue(unused);
                
                needsUpdate = true;
                break;
            }
        }

        // Trigger an update on the message thread, if needed
        if (needsUpdate)
            triggerAsyncUpdate();
    }

    void handleAsyncUpdate() override
    {
        // Emits all values from the queue
        T value(dummy);
        while (queue.try_dequeue(value))
            detail::LockFreeSourceBase<T>::subject.onNext(value);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LockFreeSource)
};
