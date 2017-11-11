#include "../Other/TestPrefix.h"


TEST_CASE("Observer",
          "[Observer]")
{
    IT("Emits values pushed to onNext")
    {
        auto o = Observable<var>::create([](Observer<var> observer) {
            observer.onNext(3);
            observer.onNext("Hello");
        });

        Array<var> values;
        ReaX_CollectValues(o, values);

        ReaX_RequireValues(values, var(3), var("Hello"));
    }

    IT("emits an error when calling onError")
    {
        auto o = Observable<int>::create([](Observer<int> observer) {
            observer.onError(std::exception_ptr());
        });

        bool onErrorCalled = false;
        o.subscribe([](int) {}, [&](std::exception_ptr) { onErrorCalled = true; });
        REQUIRE(onErrorCalled);
    }

    IT("notifies onCompleted")
    {
        auto o = Observable<float>::create([](Observer<float> observer) {
            observer.onCompleted();
        });

        bool completed = false;
        o.subscribe([](float) {}, [](std::exception_ptr) {}, [&]() { completed = true; });

        REQUIRE(completed);
    }

    IT("can subscribe to an Observable")
    {
        DisposeBag disposeBag;

        // Create subject
        BehaviorSubject<var> subject("Initial Value");
        CHECK(subject.getValue() == "Initial Value");

        // Collect values from subject
        Array<var> values;
        ReaX_CollectValues(subject, values);

        // Subscribe observer to some observable
        Observer<var> observer = subject;
        Observable<var>::from({ 3, 4, 5 }).subscribe(observer).disposedBy(disposeBag);

        // Subject should have received values from Observable
        ReaX_RequireValues(values, var("Initial Value"), var(3), var(4), var(5));
        REQUIRE(subject.getValue() == var(5));
    }
}

TEST_CASE("Observer contravariance",
          "[Observer]")
{
    CONTEXT("implicit conversion")
    {
        PublishSubject<float> floats;
        PublishSubject<int> ints;
        PublishSubject<var> vars;
        PublishSubject<String> strings;
        
        IT("can convert from var to float")
        {
            Array<var> values;
            Observer<float> o = vars;
            ReaX_CollectValues(vars, values);
            o.onNext(14.57f);
            
            ReaX_RequireValues(values, 14.57f);
        }
        
        IT("can convert from float to var")
        {
            Array<float> values;
            Observer<var> o = floats;
            ReaX_CollectValues(floats, values);
            o.onNext(var(85.2f));
            
            ReaX_RequireValues(values, 85.2f);
        }
        
        IT("can convert from var to String")
        {
            Array<var> values;
            Observer<String> o = vars;
            ReaX_CollectValues(vars, values);
            o.onNext("Hello world!");
            
            ReaX_RequireValues(values, "Hello world!");
        }
        
        IT("can convert from float to int")
        {
            Array<int> values;
            Observer<float> o = ints;
            ReaX_CollectValues(ints, values);
            o.onNext(15.67f);
            
            ReaX_RequireValues(values, 15);
        }
    }
    
    CONTEXT("inheritance")
    {
        struct Base
        {
            Base(int a)
            : a(a) {}
            
            int a;
            
            bool operator==(const Base& rhs) const { return (a == rhs.a); }
        };
        struct Derived : Base
        {
            Derived(int a, int b)
            : Base(a), b(b) {}
            
            int b;
        };
        
        PublishSubject<Base> bases;
        
        IT("can convert from Deriveds to Bases")
        {
            Array<Base> values;
            Observer<Derived> o = bases;
            ReaX_CollectValues(bases, values);
            o.onNext(Derived(205, 1002));
            
            ReaX_RequireValues(values, Base(205));
        }
    }
}
