Disposable::Disposable(detail::any&& wrapped)
: wrapped(std::move(wrapped))
{}

void Disposable::dispose() const
{
    wrapped.get<rxcpp::subscription>().unsubscribe();
}

void Disposable::disposedBy(DisposeBag& disposeBag)
{
    disposeBag.insert(std::move(*this));
}
