detail::ObserverImpl::ObserverImpl(const varx::any& wrapped)
: wrapped(wrapped)
{}

void detail::ObserverImpl::onNext(const juce::var &next) const
{
    wrapped.get<rxcpp_subscriber>().on_next(next);
}

void detail::ObserverImpl::onError(Error error) const
{
    wrapped.get<rxcpp_subscriber>().on_error(error);
}

void detail::ObserverImpl::onCompleted() const
{
    wrapped.get<rxcpp_subscriber>().on_completed();
}
