DisposeBag::DisposeBag()
: wrapped(rxcpp::composite_subscription()) {}

DisposeBag::~DisposeBag()
{
    wrapped.get<rxcpp::composite_subscription>().unsubscribe();
}

void DisposeBag::insert(Disposable&& disposable)
{
    wrapped.get<rxcpp::composite_subscription>().add(disposable.wrapped.get<rxcpp::subscription>());
}
