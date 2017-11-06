#pragma once

/**
 A Subject is an Observer and an Observable in one. Pushing an item to its Observer side causes the Observable side to emit that item.
 
 ​ **If you copy the Observer and Observable side, and destroy the Subject, the Observer and Observable remain connected!** So pushing items to the Observer will still cause the Observable to emit.
 
 For an introduction to Subjects, please refer to http://reactivex.io/documentation/subject.html.
 */
template<typename T>
class Subject : public Observer<T>, public Observable<T>
{
protected:
    ///@cond INTERNAL
    const detail::SubjectImpl impl;

    explicit Subject(detail::SubjectImpl&& impl)
    : Observer<T>(impl),
      Observable<T>(impl),
      impl(impl)
    {}
    ///@endcond
};


/**
 A subject that starts with an initial item. On subscribe, it emits the most recently emitted item. It then continues to emit any items that are passed to onNext.
 
 For an introduction to Subjects, please refer to http://reactivex.io/documentation/subject.html.
 */
template<typename T>
class BehaviorSubject : public Subject<T>
{
public:
    /// Creates a new instance with a given initial item 
    explicit BehaviorSubject(const T& initial)
    : Subject<T>(detail::SubjectImpl::MakeBehaviorSubjectImpl(detail::any(initial)))
    {}

    /// Returns the most recently emitted item. If no items have been emitted, it returns the initial item. 
    T getLatestItem() const
    {
        return Subject<T>::impl.getLatestItem().template get<T>();
    }

private:
    JUCE_LEAK_DETECTOR(BehaviorSubject)
};


/**
 A subject that initially doesn't have a value. It does not emit an item on subscribe, and emits only those items that are passed to onNext *after the time of the subscription*.
 
 For an introduction to Subjects, please refer to http://reactivex.io/documentation/subject.html.
 */
template<typename T>
class PublishSubject : public Subject<T>
{
public:
    /// Creates a new instance. 
    PublishSubject()
    : Subject<T>(detail::SubjectImpl::MakePublishSubjectImpl())
    {}

private:
    JUCE_LEAK_DETECTOR(PublishSubject)
};

/**
 A Subject that, on every new subscription, notifies the Observer with all of the items that were emitted since the ReplaySubject was created. It then continues to emit any items that are passed to onNext.
 
 For an introduction to Subjects, please refer to http://reactivex.io/documentation/subject.html.
 */
template<typename T>
class ReplaySubject : public Subject<T>
{
public:
    /**
     Creates a new instance.
     
     The `bufferSize` is the maximum number of items to remember and replay. Defaults to remembering "all" items (within memory boundaries). The buffer size is increased as items are emitted (not allocated upfront).
     */
    explicit ReplaySubject(size_t bufferSize = std::numeric_limits<size_t>::max())
    : Subject<T>(detail::SubjectImpl::MakeReplaySubjectImpl(bufferSize))
    {}

private:
    JUCE_LEAK_DETECTOR(ReplaySubject)
};
