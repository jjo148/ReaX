namespace detail {
ObserverImpl::ObserverImpl(any&& wrapped)
: wrapped(std::move(wrapped))
{}

void ObserverImpl::onNext(any&& next) const
{
    wrapped.get<rxcpp_subscriber>().on_next(std::move(next));
}

void ObserverImpl::onError(Error error) const
{
    wrapped.get<rxcpp_subscriber>().on_error(error);
}

void ObserverImpl::onCompleted() const
{
    wrapped.get<rxcpp_subscriber>().on_completed();
}
}
