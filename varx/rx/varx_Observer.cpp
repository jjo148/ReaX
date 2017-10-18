ObserverBase::ObserverBase(const std::shared_ptr<Impl>& impl)
: impl(impl) {}

void ObserverBase::onNext(const juce::var& next) const
{
    impl->wrapped.on_next(next);
}

void ObserverBase::onError(Error error) const
{
    impl->wrapped.on_error(error);
}

void ObserverBase::onCompleted() const
{
    impl->wrapped.on_completed();
}
