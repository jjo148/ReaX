Subscription::Subscription(detail::any&& wrapped)
: wrapped(std::move(wrapped))
{}

void Subscription::unsubscribe() const
{
    wrapped.get<rxcpp::subscription>().unsubscribe();
}

void Subscription::disposedBy(DisposeBag& disposeBag)
{
    disposeBag.insert(*this);
}
