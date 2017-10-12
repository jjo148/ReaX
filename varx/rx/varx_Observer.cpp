Observer::Observer(const std::shared_ptr<Impl>& impl)
: impl(impl) {}

void Observer::onNext(const juce::var& next) const
{
    impl->wrapped.on_next(next);
}

void Observer::onError(Error error) const
{
    impl->wrapped.on_error(error);
}

void Observer::onCompleted() const
{
    impl->wrapped.on_completed();
}
