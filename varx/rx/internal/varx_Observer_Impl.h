#pragma once

namespace detail {
    struct ObserverImpl
    {
        ObserverImpl(const any& wrapped);
        void onNext(any&& next) const;
        void onError(Error error) const;
        void onCompleted() const;
        
        const any wrapped;
    };
}
