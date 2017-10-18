#warning impl can probably passed by rvalue ref
SubjectBase::SubjectBase(const SubjectBase::Impl_ptr& impl)
: impl(impl)
{}

ObserverBase::Impl_ptr SubjectBase::asObserver() const
{
    return std::make_shared<ObserverBase::Impl>(impl->getSubscriber());
}

ObservableBase::Impl_ptr SubjectBase::asObservable() const
{
    return std::make_shared<ObservableBase::Impl>(impl->asObservable());
}

SubjectBase::Impl_ptr SubjectBase::MakeBehaviorSubjectImpl(var&& initial)
{
    return std::make_shared<BehaviorSubjectImpl>(std::move(initial));
}

SubjectBase::Impl_ptr SubjectBase::MakePublishSubjectImpl()
{
    return std::make_shared<PublishSubjectImpl>();
}

SubjectBase::Impl_ptr SubjectBase::MakeReplaySubjectImpl(size_t bufferSize)
{
    return std::make_shared<ReplaySubjectImpl>(bufferSize);
}

var SubjectBase::getLatestItem() const
{
    return impl->getLatestItem();
}
