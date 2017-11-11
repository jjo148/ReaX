#pragma once

namespace detail {
template<typename T>
class LockFreeTargetBase
{
protected:
    LockFreeTargetBase()
    {
        subject.subscribe([this](const T& newValue) {
                   queue.enqueue(newValue);
               })
            .disposedBy(disposeBag);
    }

    moodycamel::ConcurrentQueue<T> queue;
    PublishSubject<T> subject;
    DisposeBag disposeBag;
};
}

/**
 An `Observer` that puts all retrieved values in a lock-free queue. The queue can be accessed from another thread without locking.
 
 Useful to transfer data from a non-realtime thread to a realtime thread.
 */
template<typename T>
class LockFreeTarget : private detail::LockFreeTargetBase<T>, public Observer<T>
{
public:
    LockFreeTarget()
    : Observer<T>(detail::LockFreeTargetBase<T>::subject)
    {}

    /**
     Dequeues the next value from the queue (if it's non-empty) and assigns it to `value`.
     
     Returns `true` iff the queue was non-empty. If it returns `false`, then `value` is untouched.
     
     Does not lock. Uses move-assignment if `value` supports it, copy-assignment otherwise. Does not allocate dynamic memory, unless `value` does during assigment.
     
     May be called from any thread, including the thread on which the `Observer` retrieves values.
     */
    template<typename U>
    bool tryDequeue(U& value)
    {
        return detail::LockFreeTargetBase<T>::queue.try_dequeue(value);
    }

    /**
     Dequeues all values from the queue (if it's non-empty) and assigns the last (newest) value to `value`.
     
     Returns `true` iff the queue was non-empty. If it returns `false`, then `value` is untouched.
     
     Does not lock. Uses move-assignment if `value` supports it, copy-assignment otherwise. Does not allocate dynamic memory, unless `value` does during assigment.
     
     May be called from any thread, including the thread on which the `Observer` retrieves values.
     */
    template<typename U>
    bool tryDequeueAll(U& value)
    {
        bool hadValues = false;
        while (tryDequeue(value))
            hadValues = true;

        return hadValues;
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LockFreeTarget)
};
