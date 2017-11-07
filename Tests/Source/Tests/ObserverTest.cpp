#include "../Other/TestPrefix.h"


TEST_CASE("Observer",
          "[Observer]")
{
    IT("Emits values pushed to onNext")
    {
        auto o = Observable<>::create<var>([](Observer<var> observer) {
            observer.onNext(3);
            observer.onNext("Hello");
        });

        Array<var> values;
        varxCollectValues(o, values);

        varxRequireValues(values, var(3), var("Hello"));
    }

    IT("emits an error when calling onError")
    {
        auto o = Observable<>::create<int>([](Observer<int> observer) {
            observer.onError(std::exception_ptr());
        });

        bool onErrorCalled = false;
        o.subscribe([](int) {}, [&](std::exception_ptr) { onErrorCalled = true; });
        REQUIRE(onErrorCalled);
    }

    IT("notifies onCompleted")
    {
        auto o = Observable<>::create<float>([](Observer<float> observer) {
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
        varxCollectValues(subject, values);

        // Subscribe observer to some observable
        Observer<var> observer = subject;
        Observable<>::from<var>({ 3, 4, 5 }).subscribe(observer).disposedBy(disposeBag);

        // Subject should have received values from Observable
        varxRequireValues(values, var("Initial Value"), var(3), var(4), var(5));
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
            varxCollectValues(vars, values);
            o.onNext(14.57f);
            
            varxRequireValues(values, 14.57f);
        }
        
        IT("can convert from float to var")
        {
            Array<float> values;
            Observer<var> o = floats;
            varxCollectValues(floats, values);
            o.onNext(var(85.2f));
            
            varxRequireValues(values, 85.2f);
        }
        
        IT("can convert from var to String")
        {
            Array<var> values;
            Observer<String> o = vars;
            varxCollectValues(vars, values);
            o.onNext("Hello world!");
            
            varxRequireValues(values, "Hello world!");
        }
        
        IT("can convert from float to int")
        {
            Array<int> values;
            Observer<float> o = ints;
            varxCollectValues(ints, values);
            o.onNext(15.67f);
            
            varxRequireValues(values, 15);
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
            varxCollectValues(bases, values);
            o.onNext(Derived(205, 1002));
            
            varxRequireValues(values, Base(205));
        }
    }
}
