#pragma once

class SubjectBase
{
private:
    friend class BehaviorSubjectImpl;
    friend class PublishSubjectImpl;
    friend class ReplaySubjectImpl;
    
    template<typename T>
    friend class BehaviorSubject;
    template<typename T>
    friend class PublishSubject;
    template<typename T>
    friend class ReplaySubject;

    class Impl;
    typedef std::shared_ptr<Impl> Impl_ptr;
    explicit SubjectBase(const Impl_ptr& impl);
    Impl_ptr impl;
    
    ObserverBase::Impl_ptr asObserver() const;
    ObservableBase::Impl_ptr asObservable() const;

    static Impl_ptr MakeBehaviorSubjectImpl(juce::var&& initial);
    static Impl_ptr MakePublishSubjectImpl();
    static Impl_ptr MakeReplaySubjectImpl(size_t bufferSize);

    juce::var getLatestItem() const;
};


/**
 A subject that starts with an initial item. On subscribe, it emits the most recently emitted item. It then continues to emit any items that are passed to onNext.
 */
template<typename T>
class BehaviorSubject : private SubjectBase, public Observer<T>, public Observable<T>
{
public:
    /** Creates a new instance with a given initial item */
    explicit BehaviorSubject(const T& initial)
    : SubjectBase(MakeBehaviorSubjectImpl(toVar(initial))),
      Observer<T>(asObserver()),
      Observable<T>(asObservable())
    {}

    /** Returns the most recently emitted item. If no items have been emitted, it returns the initial item. */
    T getLatestItem() const
    {
        return fromVar<T>(SubjectBase::getLatestItem());
    }

private:
    JUCE_LEAK_DETECTOR(BehaviorSubject)
};


/**
 A subject that initially doesn't have a value. It does not emit an item on subscribe, and emits only those items that are passed to onNext *after the time of the disposable*.
 */
template<typename T>
class PublishSubject : private SubjectBase, public Observer<T>, public Observable<T>
{
public:
    /** Creates a new instance. */
    PublishSubject()
    : SubjectBase(MakePublishSubjectImpl()),
      Observer<T>(asObserver()),
      Observable<T>(asObservable())
    {}

private:
    JUCE_LEAK_DETECTOR(PublishSubject)
};

/**
 A Subject that, on every new disposable, notifies the Observer with all of the items that were emitted since the ReplaySubject was created. It then continues to emit any items that are passed to onNext.
 */
template<typename T>
class ReplaySubject : private SubjectBase, public Observer<T>, public Observable<T>
{
public:
    /**
     Creates a new instance.
     
     The `bufferSize` is the maximum number of items to remember and replay. Defaults to remembering "all" items (within memory boundaries). The buffer size is increased as items are emitted (not allocated upfront).
     */
    explicit ReplaySubject(size_t bufferSize = std::numeric_limits<size_t>::max())
    : SubjectBase(MakeReplaySubjectImpl(bufferSize)),
    Observer<T>(asObserver()),
    Observable<T>(asObservable())
    {}
    
private:
    JUCE_LEAK_DETECTOR(ReplaySubject)
};
