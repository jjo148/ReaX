#warning impl can probably passed by rvalue ref
SubjectBase::SubjectBase(const SubjectBase::Impl_ptr& impl)
: impl(impl)
{}

Observer::Impl_ptr SubjectBase::asObserver() const
{
    return std::make_shared<Observer::Impl>(impl->getSubscriber());
}

ObservableBase::Impl_ptr SubjectBase::asObservable() const
{
    return std::make_shared<ObservableBase::Impl>(impl->asObservable());
}

SubjectBase::Impl_ptr SubjectBase::MakeBehaviorSubjectImpl(var&& initial)
{
    return std::make_shared<TypedBehaviorSubjectImpl>(std::move(initial));
}

SubjectBase::Impl_ptr SubjectBase::MakePublishSubjectImpl()
{
    return std::make_shared<TypedPublishSubjectImpl>();
}

SubjectBase::Impl_ptr SubjectBase::MakeReplaySubjectImpl(size_t bufferSize)
{
    return std::make_shared<TypedReplaySubjectImpl>(bufferSize);
}

var SubjectBase::getLatestItem() const
{
    return impl->getLatestItem();
}
