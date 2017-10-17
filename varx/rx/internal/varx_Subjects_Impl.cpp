#include "varx_Subjects_Impl.h"

var Subject::Impl::getLatestItem() const
{
    jassertfalse;
    return var::undefined();
}

BehaviorSubjectImpl::BehaviorSubjectImpl(const juce::var& initial)
: wrapped(initial)
{}

rxcpp::subscriber<var> BehaviorSubjectImpl::getSubscriber() const
{
    return wrapped.get_subscriber();
}

rxcpp::observable<var> BehaviorSubjectImpl::asObservable() const
{
    return wrapped.get_observable();
}

var BehaviorSubjectImpl::getLatestItem() const
{
    return wrapped.get_value();
}

PublishSubjectImpl::PublishSubjectImpl()
{}

rxcpp::subscriber<var> PublishSubjectImpl::getSubscriber() const
{
    return wrapped.get_subscriber();
}

rxcpp::observable<var> PublishSubjectImpl::asObservable() const
{
    return wrapped.get_observable();
}

ReplaySubjectImpl::ReplaySubjectImpl(size_t bufferSize)
: wrapped(bufferSize, rxcpp::identity_immediate())
{}

rxcpp::subscriber<var> ReplaySubjectImpl::getSubscriber() const
{
    return wrapped.get_subscriber();
}

rxcpp::observable<var> ReplaySubjectImpl::asObservable() const
{
    return wrapped.get_observable();
}


var SubjectBase::Impl::getLatestItem() const
{
    jassertfalse;
    return var::undefined();
}

TypedBehaviorSubjectImpl::TypedBehaviorSubjectImpl(juce::var&& initial)
: wrapped(initial)
{}

rxcpp::subscriber<var> TypedBehaviorSubjectImpl::getSubscriber() const
{
    return wrapped.get_subscriber();
}

rxcpp::observable<var> TypedBehaviorSubjectImpl::asObservable() const
{
    return wrapped.get_observable();
}

var TypedBehaviorSubjectImpl::getLatestItem() const
{
    return wrapped.get_value();
}

TypedPublishSubjectImpl::TypedPublishSubjectImpl()
{}

rxcpp::subscriber<var> TypedPublishSubjectImpl::getSubscriber() const
{
    return wrapped.get_subscriber();
}

rxcpp::observable<var> TypedPublishSubjectImpl::asObservable() const
{
    return wrapped.get_observable();
}

TypedReplaySubjectImpl::TypedReplaySubjectImpl(size_t bufferSize)
: wrapped(bufferSize, rxcpp::identity_immediate())
{}

rxcpp::subscriber<var> TypedReplaySubjectImpl::getSubscriber() const
{
    return wrapped.get_subscriber();
}

rxcpp::observable<var> TypedReplaySubjectImpl::asObservable() const
{
    return wrapped.get_observable();
}
