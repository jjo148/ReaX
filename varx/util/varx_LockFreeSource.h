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
 An Observable that receives items from a realtime thread (like the audio thread) and emits those items on the JUCE message thread.
 
 Call asObservable() to get the Observable, subscribe to it, etc. Then call LockFreeSource::onNext on the realtime thread to emit items.
 */
template<typename T>
class LockFreeSource : private detail::LockFreeSourceBase<T>, private juce::AsyncUpdater, public Observable<T>
{
public:
    /**
     Determines what should be done if the queue is full. This happens when onNext() is called too often in a row, without the message thread taking items from the queue in between.
     
     Allocate: Allocate dynamic memory to make room for more items. You will most likely call onNext() on the realtime thread, so only use this if you cannot drop any items, and make sure to pick a sufficiently large queueCapacity.
     
     DropLatest: Never allocate memory. If the queue is full, onNext() does nothing, and new items are discarded.
     
     DropOldest: Never allocate memory. If the queue is full, the oldest item is removed to make room for a new item. If you only ever need the latest state, you can use this policy with a queueCapacity of 1.
     */
    enum class CongestionPolicy {
        Allocate,
        DropLatest,
        DropOldest
    };

    /**
     Creates a new instance.
     
     The queueCapacity must be > 0. If you have to use CongestionPolicy::Allocate, use a large capacity, to make dynamic allocation on the audio thread as unlikely as possible.
     */
    explicit LockFreeSource(size_t queueCapacity)
    : Observable<T>(detail::LockFreeSourceBase<T>::subject),
      queue(queueCapacity)
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

    template<typename U>
    inline void _onNext(U&& item, CongestionPolicy congestionPolicy)
    {
        switch (congestionPolicy) {
            // If allocation is allowed, just enqueue the item, allowing the queue to allocate memory if needed.
            case CongestionPolicy::Allocate:
                queue.enqueue(std::forward<U>(item));
                break;

            // If the latest item may be dropped, just try to enqueue (without allocating), and do nothing if it fails.
            case CongestionPolicy::DropLatest:
                queue.try_enqueue(std::forward<U>(item));
                break;

            // If the oldest item may be dropped, try to enqueue (without allocating), and remove the oldest item if needed.
            case CongestionPolicy::DropOldest: {
                T unused;
                // Cannot use std::forward here: Item must not be moved because try_enqueue may be called multiple times.
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
        T item;
        while (queue.try_dequeue(item))
            detail::LockFreeSourceBase<T>::subject.onNext(item);
    }

    JUCE_LEAK_DETECTOR(LockFreeSource)
};
