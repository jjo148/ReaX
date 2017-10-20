namespace detail {
ObserverImpl::ObserverImpl(const any& wrapped)
: wrapped(wrapped)
{}

void ObserverImpl::onNext(any&& next) const
{
    wrapped.get<rxcpp::subscriber<any>>().on_next(std::move(next));
}

void ObserverImpl::onError(std::exception_ptr error) const
{
    wrapped.get<rxcpp::subscriber<any>>().on_error(error);
}

void ObserverImpl::onCompleted() const
{
    wrapped.get<rxcpp::subscriber<any>>().on_completed();
}
}
