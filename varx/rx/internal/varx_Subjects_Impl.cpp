using std::move;

namespace detail {
SubjectImpl SubjectImpl::MakeBehaviorSubjectImpl(any&& initial)
{
    rxcpp::subjects::behavior<any> subject(initial);
    auto observer = subject.get_subscriber().as_dynamic();
    auto observable = subject.get_observable().as_dynamic();

    return SubjectImpl(any(move(subject)), any(move(observer)), any(move(observable)));
}

SubjectImpl SubjectImpl::MakePublishSubjectImpl()
{
    rxcpp::subjects::subject<any> subject;
    auto observer = subject.get_subscriber().as_dynamic();
    auto observable = subject.get_observable().as_dynamic();

    return SubjectImpl(any(move(subject)), any(move(observer)), any(move(observable)));
}

SubjectImpl SubjectImpl::MakeReplaySubjectImpl(size_t bufferSize)
{
    rxcpp::subjects::replay<any, rxcpp::identity_one_worker> subject(bufferSize, rxcpp::identity_immediate());
    auto observer = subject.get_subscriber().as_dynamic();
    auto observable = subject.get_observable().as_dynamic();

    return SubjectImpl(any(move(subject)), any(move(observer)), any(move(observable)));
}

any SubjectImpl::getLatestItem() const
{
    return wrapped.get<rxcpp::subjects::behavior<any>>().get_value();
}

SubjectImpl::SubjectImpl(any&& subject, any&& observer, any&& observable)
: ObserverImpl(move(observer)),
  ObservableImpl(move(observable)),
  wrapped(move(subject))
{}
}
