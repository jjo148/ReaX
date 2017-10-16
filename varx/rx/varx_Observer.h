#pragma once

class Observable;

/**
    Retrieves items. You can call onNext to notify the Observer with a new item.
 
    An Observer does **not** automatically call onCompleted when it's destroyed.
 
    @see Subject, Observable::create
 */
class Observer
{
public:
    /** Notifies the Observer with a new item. */
    template<typename T>
    void onNext(T&& next) const
    {
        _onNext(toVar(std::forward<T>(next)));
    }

    /** Notifies the Observer that an error has occurred. */
    void onError(Error error) const;

    /** Notifies the Observer that no more values will be pushed. */
    void onCompleted() const;

private:
    friend class Subject;
    friend class Observable;
    struct Impl;
    explicit Observer(const std::shared_ptr<Impl>& impl);
    std::shared_ptr<Impl> impl;
    
    void _onNext(const juce::var& next) const;

    JUCE_LEAK_DETECTOR(Observer)
};
