#pragma once

namespace detail {
    struct ObserverImpl
    {
        ObserverImpl(any&& wrapped);
        void onNext(any&& next) const;
        void onError(Error error) const;
        void onCompleted() const;
        
        const any wrapped;
    };
}
