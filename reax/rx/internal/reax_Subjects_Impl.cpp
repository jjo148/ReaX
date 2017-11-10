namespace {
template<typename SubjectType, typename... Args>
detail::SubjectImpl MakeSubjectImpl(Args&&... args)
{
    // Need to use shared_ptr here, so we can call get_subscriber() and get_observable() on the same instance that we store in the SubjectImpl.
    auto subject = std::make_shared<SubjectType>(std::forward<Args>(args)...);
    return detail::SubjectImpl(any(subject), any(subject->get_subscriber().as_dynamic()), any(subject->get_observable().as_dynamic()));
}
}

namespace detail {
SubjectImpl SubjectImpl::MakeBehaviorSubjectImpl(any&& initial)
{
    return MakeSubjectImpl<rxcpp::subjects::behavior<any>>(std::move(initial));
}

SubjectImpl SubjectImpl::MakePublishSubjectImpl()
{
    return MakeSubjectImpl<rxcpp::subjects::subject<any>>();
}

SubjectImpl SubjectImpl::MakeReplaySubjectImpl(size_t bufferSize)
{
    return MakeSubjectImpl<rxcpp::subjects::replay<any, rxcpp::identity_one_worker>>(bufferSize, rxcpp::identity_immediate());
}

any SubjectImpl::getValue() const
{
    return wrapped.get<std::shared_ptr<rxcpp::subjects::behavior<any>>>()->get_value();
}

SubjectImpl::SubjectImpl(const any& subject, const any& observer, const any& observable)
: ObserverImpl(observer),
  ObservableImpl(observable),
  wrapped(subject)
{}
}
