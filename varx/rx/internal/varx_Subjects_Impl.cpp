namespace detail {
SubjectImpl SubjectImpl::MakeBehaviorSubjectImpl(any&& initial)
{
    auto subject = std::make_shared<rxcpp::subjects::behavior<any>>(std::move(initial));
    return SubjectImpl(any(subject), any(subject->get_subscriber().as_dynamic()), any(subject->get_observable().as_dynamic()));
}

SubjectImpl SubjectImpl::MakePublishSubjectImpl()
{
    auto subject = std::make_shared<rxcpp::subjects::subject<any>>();
    return SubjectImpl(any(subject), any(subject->get_subscriber().as_dynamic()), any(subject->get_observable().as_dynamic()));
}

SubjectImpl SubjectImpl::MakeReplaySubjectImpl(size_t bufferSize)
{
    auto subject = std::make_shared<rxcpp::subjects::replay<any, rxcpp::identity_one_worker>>(bufferSize, rxcpp::identity_immediate());
    return SubjectImpl(any(subject), any(subject->get_subscriber().as_dynamic()), any(subject->get_observable().as_dynamic()));
}

any SubjectImpl::getLatestItem() const
{
    return wrapped.get<std::shared_ptr<rxcpp::subjects::behavior<any>>>()->get_value();
}

SubjectImpl::SubjectImpl(const any& subject, const any& observer, const any& observable)
: ObserverImpl(observer),
  ObservableImpl(observable),
  wrapped(subject)
{}
}
