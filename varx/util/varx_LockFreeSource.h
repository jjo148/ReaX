#pragma once

/**
 An Observable that receives items from a realtime thread (like the audio thread) and emits those items on the JUCE message thread.
 
 Call asObservable() to get the Observable, subscribe to it, etc. Then call LockFreeSource::onNext on the realtime thread to emit items.
 */
template<typename T>
class LockFreeSource : private juce::AsyncUpdater
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
    : queue(queueCapacity)
    {
        // The queue capacity must be > 0.
        jassert(queueCapacity > 0);
    }

    /**
     Adds an item that will be emitted from the Observable.
     
     The congestionPolicy determines what to do if the queue is full. @see CongestionPolicy
     */
    inline void onNext(const T& item, CongestionPolicy congestionPolicy)
    {
        switch (congestionPolicy) {
            // If allocation is allowed, just enqueue the item, allowing the queue to allocate memory if needed.
            case CongestionPolicy::Allocate:
                queue.enqueue(item);
                break;

            // If the latest item may be dropped, just try to enqueue (without allocating), and do nothing if it fails.
            case CongestionPolicy::DropLatest:
                queue.try_enqueue(item);
                break;

            // If the oldest item may be dropped, try to enqueue (without allocating), and remove the oldest item if needed.
            case CongestionPolicy::DropOldest: {
                T unused;
                while (!queue.try_enqueue(item))
                    queue.try_dequeue(unused);
                break;
            }
        }

        // Trigger an update on the message thread
        triggerAsyncUpdate();
    }
    
#warning Should probably just inherit from Observable<T>
    /** Returns an Observable that emits every item added through onNext(). Items are emitted on the JUCE message thread. */
    inline TypedObservable<T> asObservable() const { return subject; }

    /** Calls asObservable(). */
    operator TypedObservable<T>() const { return asObservable(); }

private:
    moodycamel::ConcurrentQueue<T> queue;
    TypedPublishSubject<T> subject;

    void handleAsyncUpdate() override
    {
        // Forward all items from the queue to the subject
        T item;
        while (queue.try_dequeue(item))
            subject.onNext(item);
    }

    JUCE_LEAK_DETECTOR(LockFreeSource)
};
