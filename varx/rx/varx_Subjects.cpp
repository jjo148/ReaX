Subject::Subject(const std::shared_ptr<Impl>& impl)
: Observer(std::make_shared<Observer::Impl>(impl->getSubscriber())),
  Observable(std::make_shared<Observable::Impl>(impl->asObservable())),
  impl(impl) {}

Observable Subject::asObservable() const
{
    return *this;
}

Observer Subject::asObserver() const
{
    return *this;
}

std::shared_ptr<Subject::Impl> BehaviorSubject::CreateImpl(const juce::var& initial)
{
    return std::make_shared<BehaviorSubjectImpl>(initial);
}

var BehaviorSubject::getLatestItem() const
{
    return impl->getLatestItem();
}

PublishSubject::PublishSubject()
: Subject(std::make_shared<PublishSubjectImpl>()) {}

ReplaySubject::ReplaySubject(size_t bufferSize)
: Subject(std::make_shared<ReplaySubjectImpl>(bufferSize)) {}

const size_t ReplaySubject::MaxBufferSize = std::numeric_limits<size_t>::max();













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
