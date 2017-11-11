#pragma once

namespace detail {
struct SubjectImpl : public ObserverImpl, public ObservableImpl
{
    static SubjectImpl MakeBehaviorSubjectImpl(any&& initial);
    static SubjectImpl MakePublishSubjectImpl();
    static SubjectImpl MakeReplaySubjectImpl(size_t bufferSize);

    any getValue() const;

    explicit SubjectImpl(const any& subject, const any& observer, const any& observable);
    
    const any wrapped;
};
}
