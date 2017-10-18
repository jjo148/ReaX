#pragma once

/**
    A Subject is an Observer and an Observable at the same time. When Observer::onNext is called, the Observable emits an item.
 
    It does **not** automatically call onCompleted when it's destroyed.
 */
class Subject : public Observer, public Observable
{
public:
    /**
        Returns an Observable that emits an item whenever onNext is called on this subject.
     */
    Observable asObservable() const;

    /**
        Returns the Observer side. If you call onNext on this Observer, this subject's Observable side will emit an item.
     */
    Observer asObserver() const;

private:
    friend class BehaviorSubject;
    friend class BehaviorSubjectImpl;
    friend class PublishSubject;
    friend class PublishSubjectImpl;
    friend class ReplaySubject;
    friend class ReplaySubjectImpl;
    friend class Observable;
    struct Impl;
    explicit Subject(const std::shared_ptr<Impl>& impl);
    std::shared_ptr<Impl> impl;

    JUCE_LEAK_DETECTOR(Subject)
};


/**
    A subject that starts with an initial item. On subscribe, it emits the most recently emitted item. It then continues to emit any items that are passed to onNext.
 */
class BehaviorSubject : public Subject
{
public:
    /** Creates a new instance with a given initial item */
    template<typename T>
    explicit BehaviorSubject(T&& initial)
    : Subject(CreateImpl(toVar(std::forward<T>(initial))))
    {}

    /** Returns the most recently emitted item. If no items have been emitted, it returns the initial item. */
    juce::var getLatestItem() const;

private:
    explicit BehaviorSubject(const juce::var& initial);
    static std::shared_ptr<Impl> CreateImpl(const juce::var& initial);

    JUCE_LEAK_DETECTOR(BehaviorSubject)
};


/**
    A subject that initially doesn't have a value. It does not emit an item on subscribe, and emits only those items that are passed to onNext *after the time of the disposable*.
 */
class PublishSubject : public Subject
{
public:
    /** Creates a new instance. */
    PublishSubject();

private:
    JUCE_LEAK_DETECTOR(PublishSubject)
};

/**
    A Subject that, on every new disposable, notifies the Observer with all of the items that were emitted since the ReplaySubject was created. It then continues to emit any items that are passed to onNext.
 */
class ReplaySubject : public Subject
{
public:
    /**
        Creates a new instance.
     
        The `bufferSize` is the maximum number of items to remember and replay. Pass ReplaySubject::MaxBufferSize if you want all items to be remembered. The buffer size is increased as items are emitted (not allocated upfront).
     */
    explicit ReplaySubject(size_t bufferSize = MaxBufferSize);

    /**
        The maximum number of items that can be remembered by this class. You can pass this to ReplaySubject::ReplaySubject to remember "all" items (within memory boundaries).
     
        The buffer size is increased as items are emitted (not allocated upfront).
     */
    static const size_t MaxBufferSize;

private:
    JUCE_LEAK_DETECTOR(ReplaySubject)
};


class SubjectBase
{
private:
    friend class TypedBehaviorSubjectImpl;
    friend class TypedPublishSubjectImpl;
    friend class TypedReplaySubjectImpl;
    
    template<typename T>
    friend class TypedBehaviorSubject;
    template<typename T>
    friend class TypedPublishSubject;
    template<typename T>
    friend class TypedReplaySubject;

    class Impl;
    typedef std::shared_ptr<Impl> Impl_ptr;
    explicit SubjectBase(const Impl_ptr& impl);
    Impl_ptr impl;
    
    Observer::Impl_ptr asObserver() const;
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
class TypedBehaviorSubject : private SubjectBase, public TypedObserver<T>, public TypedObservable<T>
{
public:
    /** Creates a new instance with a given initial item */
    explicit TypedBehaviorSubject(const T& initial)
    : SubjectBase(MakeBehaviorSubjectImpl(toVar(initial))),
      TypedObserver<T>(asObserver()),
      TypedObservable<T>(asObservable())
    {}

    /** Returns the most recently emitted item. If no items have been emitted, it returns the initial item. */
    T getLatestItem() const
    {
        return fromVar<T>(SubjectBase::getLatestItem());
    }

private:
    JUCE_LEAK_DETECTOR(TypedBehaviorSubject)
};


/**
 A subject that initially doesn't have a value. It does not emit an item on subscribe, and emits only those items that are passed to onNext *after the time of the disposable*.
 */
template<typename T>
class TypedPublishSubject : private SubjectBase, public TypedObserver<T>, public TypedObservable<T>
{
public:
    /** Creates a new instance. */
    TypedPublishSubject()
    : SubjectBase(MakePublishSubjectImpl()),
      TypedObserver<T>(asObserver()),
      TypedObservable<T>(asObservable())
    {}

private:
    JUCE_LEAK_DETECTOR(TypedPublishSubject)
};

/**
 A Subject that, on every new disposable, notifies the Observer with all of the items that were emitted since the ReplaySubject was created. It then continues to emit any items that are passed to onNext.
 */
template<typename T>
class TypedReplaySubject : private SubjectBase, public TypedObserver<T>, public TypedObservable<T>
{
public:
    /**
     Creates a new instance.
     
     The `bufferSize` is the maximum number of items to remember and replay. Defaults to remembering "all" items (within memory boundaries). The buffer size is increased as items are emitted (not allocated upfront).
     */
    explicit TypedReplaySubject(size_t bufferSize = std::numeric_limits<size_t>::max())
    : SubjectBase(MakeReplaySubjectImpl(bufferSize)),
    TypedObserver<T>(asObserver()),
    TypedObservable<T>(asObservable())
    {}
    
private:
    JUCE_LEAK_DETECTOR(TypedReplaySubject)
};
