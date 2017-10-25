#pragma once

/**
 Retrieves items. You can call onNext to notify the Observer with a new item.
 
 An Observer does **not** automatically call onCompleted when it's destroyed.
 
 @see Subject, Observable::create
 */
template<typename T>
class Observer
{
public:
    /// Notifies the Observer with a new item. 
    void onNext(const T& item) const
    {
        impl.onNext(convert(detail::any(item)));
    }

    /// Notifies the Observer that an error has occurred. 
    void onError(std::exception_ptr error) const
    {
        impl.onError(error);
    }

    /// Notifies the Observer that no more values will be pushed. 
    void onCompleted() const
    {
        impl.onCompleted();
    }

    /// Contravariant constructor: If T is convertible to U, an Observer<U> is convertible to an Observer<T>. 
    template<typename U>
    Observer(const Observer<U>& other, typename std::enable_if<std::is_convertible<T, U>::value>::type* = 0)
    : Observer(other.impl, [](const detail::any& item){ return detail::any(static_cast<U>(item.get<T>())); })
    {}

protected:
    template<typename U>
    friend class Observable;
    
    ///@cond INTERNAL
    Observer(const detail::ObserverImpl& impl, const std::function<detail::any(const detail::any&)>& convert = [](const detail::any& item){ return item; })
    : impl(impl),
      convert(convert)
    {}
    ///@endcond

private:
    template<typename U>
    friend class Observer;

    const detail::ObserverImpl impl;
    const std::function<detail::any(const detail::any&)> convert;

    JUCE_LEAK_DETECTOR(Observer)
};
