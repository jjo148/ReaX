Disposable::Disposable(const std::shared_ptr<Impl>& impl)
: impl(impl) {}

void Disposable::dispose() const
{
    impl->wrapped.unsubscribe();
}

void Disposable::disposedBy(DisposeBag& disposeBag)
{
    disposeBag.insert(*this);
}
