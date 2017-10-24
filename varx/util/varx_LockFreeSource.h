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
 Determines what should be done if the queue is full. This happens when onNext() is called too often in a row, without the message thread taking items from the queue in between.
 
 Allocate: Allocate dynamic memory to make room for more items. You will most likely call onNext() on the realtime thread, so only use this if you cannot drop any items, and make sure to pick a sufficiently large queueCapacity.
 
 DropNewest: Never allocate memory. If the queue is full, onNext() does nothing, and the new item is discarded.
 
 DropOldest: Never allocate memory. If the queue is full, the oldest item is removed to make room for a new item. If you only ever need the latest state, you can use this policy with a queueCapacity of 1.
 */
enum class CongestionPolicy {
    Allocate,
    DropNewest,
    DropOldest
};

/**
 An Observable that receives items from a realtime thread (like the audio thread) and emits those items on the JUCE message thread.
 
 The item type must be copy-constructible or (preferably) move-constructible.
 
 Call asObservable() to get the Observable, subscribe to it, etc. Then call LockFreeSource::onNext on the realtime thread to emit items.
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
     Adds an item that will be emitted from the Observable.
     
     The congestionPolicy determines what to do if the queue is full. @see CongestionPolicy
     */
    inline void onNext(const T& item, CongestionPolicy congestionPolicy)
    {
        _onNext(item, congestionPolicy);
    }

    inline void onNext(T&& item, CongestionPolicy congestionPolicy)
    {
        _onNext(std::move(item), congestionPolicy);
    }
    ///@}

private:
    moodycamel::ConcurrentQueue<T> queue;
    T dummy;

    template<typename U>
    void _onNext(U&& item, CongestionPolicy congestionPolicy)
    {
        switch (congestionPolicy) {
            // If allocation is allowed, just enqueue the item, allowing the queue to allocate memory if needed.
            case CongestionPolicy::Allocate:
                queue.enqueue(std::forward<U>(item));
                break;

            // If the newest item(s) may be dropped, just try to enqueue (without allocating), and do nothing if it fails.
            case CongestionPolicy::DropNewest:
                queue.try_enqueue(std::forward<U>(item));
                break;

            // If the oldest item may be dropped, try to enqueue (without allocating), and remove the oldest item if needed.
            case CongestionPolicy::DropOldest: {
                // Try to enqueue the item. If it succeeds, there's no need to copy the dummy.
                // Cannot use std::forward here: Item must not be moved because try_enqueue may be called again (multiple times) below.
                if (queue.try_enqueue(item))
                    break;
                
                // Queue is full. Drop items from the front until there's space again:
                T unused(dummy);
                while (!queue.try_enqueue(item))
                    queue.try_dequeue(unused);
                break;
            }
        }

        // Trigger an update on the message thread
        triggerAsyncUpdate();
    }

    void handleAsyncUpdate() override
    {
        // Forward all items from the queue to the subject
        T item(dummy);
        while (queue.try_dequeue(item))
            detail::LockFreeSourceBase<T>::subject.onNext(item);
    }

    JUCE_LEAK_DETECTOR(LockFreeSource)
};
