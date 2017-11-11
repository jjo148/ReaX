DisposeBag::DisposeBag()
: wrapped(rxcpp::composite_subscription()) {}

DisposeBag::~DisposeBag()
{
    wrapped.get<rxcpp::composite_subscription>().unsubscribe();
}

void DisposeBag::insert(const Subscription& subscription)
{
    wrapped.get<rxcpp::composite_subscription>().add(subscription.wrapped.get<rxcpp::subscription>());
}
