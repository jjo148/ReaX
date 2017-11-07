#include "../Other/TestPrefix.h"


TEST_CASE("BehaviorSubject",
          "[Subject][BehaviorSubject]")
{
    BehaviorSubject<var> subject("Initial Value");

    // Subscribe to the subject's Observable
    Array<var> values;
    varxCollectValues(subject, values);

    IT("changes value when changing the Observer")
    {
        CHECK(subject.getValue() == "Initial Value");
        subject.onNext(32.55);

        REQUIRE(subject.getValue() == var(32.55));
    }

    IT("has the initial value after being created")
    {
        CHECK(subject.getValue() == "Initial Value");
        varxRequireValues(values, "Initial Value");
    }

    IT("emits when pushing a new value")
    {
        varxCheckValues(values, "Initial Value");

        subject.onNext("New Value");

        CHECK(subject.getValue() == "New Value");
        varxRequireValues(values, "Initial Value", "New Value");
    }

    IT("emits an error when calling onError")
    {
        BehaviorSubject<int> subject(17);
        bool onErrorCalled = false;
        subject.onError(std::exception_ptr());
        subject.subscribe([](int) {}, [&](std::exception_ptr) { onErrorCalled = true; });
        REQUIRE(onErrorCalled);
    }

    IT("notifies onCompleted when calling onCompleted")
    {
        bool completed = false;
        subject.subscribe([](var) {}, [](std::exception_ptr) {}, [&]() { completed = true; });
        subject.onCompleted();

        REQUIRE(completed);
    }

    IT("does not call onCompleted when destroying the subject")
    {
        auto subject = std::make_shared<BehaviorSubject<int>>(3);
        bool completed = false;
        subject->subscribe([](int) {}, [](std::exception_ptr) {}, [&]() { completed = true; });
        subject.reset();

        REQUIRE(!completed);
    }

    IT("can call onCompleted multiple times")
    {
        subject.onCompleted();
        subject.onCompleted();
        subject.onCompleted();
    }
    
    IT("can receive an initial value of a custom type, without wrapping with toVar()")
    {
        BehaviorSubject<Point<int>> subject(Point<int>(13, 556));
        REQUIRE(subject.getValue() == Point<int>(13, 556));
    }
}


TEST_CASE("PublishSubject",
          "[Subject][PublishSubject]")
{
    PublishSubject<var> subject;
    DisposeBag disposeBag;

    // Subscribe to the subject's Observable
    Array<var> values;
    varxCollectValues(subject, values);

    IT("does not emit a value if nothing has been pushed")
    {
        REQUIRE(values.isEmpty());
    }

    IT("emits when pushing a new value")
    {
        CHECK(values.isEmpty());

        subject.onNext("First Value");

        varxRequireValues(values, "First Value");
    }

    IT("does not emit previous value(s) when subscribing")
    {
        subject.onNext(1);
        subject.onNext(2);
        varxCheckValues(values, 1, 2);

        Array<var> laterValues;
        varxCollectValues(subject, laterValues);

        REQUIRE(laterValues.isEmpty());
    }

    IT("changes value when changing the Observer")
    {
        subject.onNext(32.51);
        subject.onNext(3.0);

        varxRequireValues(values, 32.51, 3.0);
    }

    IT("emits after destruction, if there's still an Observer pushing values")
    {
        auto subject = std::make_shared<PublishSubject<int>>();
        Observer<int> observer = *subject;

        varxCollectValues(*subject, values);
        subject.reset();
        observer.onNext(12345);

        varxRequireValues(values, 12345);
    }

    IT("emits an error when calling onError")
    {
        PublishSubject<int> subject;
        bool onErrorCalled = false;
        subject.onError(std::exception_ptr());
        subject.subscribe([](int) {}, [&](std::exception_ptr) { onErrorCalled = true; }).disposedBy(disposeBag);
        REQUIRE(onErrorCalled);
    }

    CONTEXT("onCompleted")
    {
        auto subject = std::make_shared<PublishSubject<int>>();
        bool completed = false;

        IT("notifies onCompleted when calling onCompleted")
        {
            subject->onCompleted();
            subject->subscribe([](int) {}, [](std::exception_ptr) {}, [&]() { completed = true; }).disposedBy(disposeBag);

            REQUIRE(completed);
        }

        IT("does not call onCompleted when destroying the subject")
        {
            subject->subscribe([](int) {}, [](std::exception_ptr) {}, [&]() { completed = true; }).disposedBy(disposeBag);
            CHECK(!completed);
            subject.reset();

            REQUIRE(!completed);
        }

        IT("can call onCompleted multiple times")
        {
            subject->onCompleted();
            subject->onCompleted();
            subject->onCompleted();
        }
    }
}


TEST_CASE("ReplaySubject",
          "[Subject][ReplaySubject]")
{
    ReplaySubject<var> subject;
    DisposeBag disposeBag;

    // Subscribe to the subject's Observable
    Array<var> values;
    varxCollectValues(subject, values);

    IT("does not emit a value if nothing has been pushed")
    {
        REQUIRE(values.isEmpty());
    }

    IT("emits when pushing a new value")
    {
        CHECK(values.isEmpty());

        subject.onNext("First Value");

        varxRequireValues(values, "First Value");
    }

    IT("emits previous values when subscribing")
    {
        subject.onNext(1);
        subject.onNext(2);
        varxCheckValues(values, 1, 2);

        Array<var> laterValues;
        varxCollectValues(subject, laterValues);

        varxRequireValues(laterValues, 1, 2);
    }

    IT("emits previous values limited by the max. buffer size")
    {
        auto subject = std::make_shared<ReplaySubject<var>>(4);

        // These should be forgotten:
        subject->onNext(17.5);
        subject->onNext("Hello!");

        // These should be remembered:
        subject->onNext(7);
        subject->onNext(28);
        subject->onNext(3);
        subject->onNext(6);

        Array<var> values;
        varxCollectValues(*subject, values);

        varxRequireValues(values, 7, 28, 3, 6);
    }

    IT("changes value when changing the Observer")
    {
        subject.onNext(32.51);
        subject.onNext(3.0);

        varxRequireValues(values, 32.51, 3.0);
    }

    IT("emits after destruction, if there's still an Observer pushing values")
    {
        auto subject = std::make_shared<ReplaySubject<int>>();
        Observer<int> observer = *subject;

        varxCollectValues(*subject, values);
        subject.reset();
        observer.onNext(12345);

        varxRequireValues(values, 12345);
    }

    IT("emits an error when calling onError")
    {
        ReplaySubject<int> subject;
        bool onErrorCalled = false;
        subject.onError(std::exception_ptr());
        subject.subscribe([](int) {}, [&](std::exception_ptr) { onErrorCalled = true; }).disposedBy(disposeBag);
        REQUIRE(onErrorCalled);
    }

    CONTEXT("onCompleted")
    {
        auto subject = std::make_shared<ReplaySubject<String>>();
        bool completed = false;

        IT("notifies onCompleted when calling onCompleted")
        {
            subject->onCompleted();
            subject->subscribe([](String) {}, [](std::exception_ptr) {}, [&]() { completed = true; }).disposedBy(disposeBag);

            REQUIRE(completed);
        }

        IT("does not call onCompleted when destroying the subject")
        {
            subject->subscribe([](String) {}, [](std::exception_ptr) {}, [&]() { completed = true; }).disposedBy(disposeBag);
            CHECK(!completed);
            subject.reset();

            REQUIRE(!completed);
        }

        IT("can call onCompleted multiple times")
        {
            subject->onCompleted();
            subject->onCompleted();
            subject->onCompleted();
        }
    }
}


TEST_CASE("onNext move overload",
          "[Subject][Observer]")
{
    CopyAndMoveConstructible::Counters counters;
    PublishSubject<CopyAndMoveConstructible> subject;
    
    IT("uses the const T& overload for lvalues")
    {
        CopyAndMoveConstructible test(&counters);
        subject.onNext(test);
        
        REQUIRE(counters.numCopyConstructions == 1);
        REQUIRE(counters.numMoveConstructions == 0);
        REQUIRE(counters.numCopyAssignments == 0);
        REQUIRE(counters.numMoveAssignments == 0);
    }
    
    IT("uses the T&& overload for rvalues")
    {
        subject.onNext(CopyAndMoveConstructible(&counters));
        
        REQUIRE(counters.numCopyConstructions == 0);
        REQUIRE(counters.numMoveConstructions == 1);
        REQUIRE(counters.numCopyAssignments == 0);
        REQUIRE(counters.numMoveAssignments == 0);
    }
}
