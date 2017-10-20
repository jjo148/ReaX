#pragma once

namespace detail {
struct SubjectImpl : public ObserverImpl, public ObservableImpl
{
    static SubjectImpl MakeBehaviorSubjectImpl(any&& initial);
    static SubjectImpl MakePublishSubjectImpl();
    static SubjectImpl MakeReplaySubjectImpl(size_t bufferSize);

    any getLatestItem() const;

private:
    const any wrapped;

    SubjectImpl(any&& subject, any&& observer, any&& observable);
};
}
