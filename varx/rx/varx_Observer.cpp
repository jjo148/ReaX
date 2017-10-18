namespace {
class ObserverImpl : public detail::ObserverImpl
{
public:
    ObserverImpl(const rxcpp::subscriber<var>& wrapped)
    : wrapped(wrapped)
    {}

    void onNext(const juce::var& next) const override { wrapped.on_next(next); }
    void onError(Error error) const override { wrapped.on_error(error); }
    void onCompleted() const override { wrapped.on_completed(); }

    const rxcpp::subscriber<var> wrapped;
};
}


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
