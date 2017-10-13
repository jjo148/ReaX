DisposeBag::DisposeBag()
: impl(std::make_shared<Impl>()) {}

DisposeBag::~DisposeBag()
{
    impl->wrapped.unsubscribe();
}

void DisposeBag::insert(const Disposable& disposable)
{
    impl->wrapped.add(disposable.impl->wrapped);
}
