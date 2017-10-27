#include "../Other/TestPrefix.h"


TEST_CASE("Observer",
          "[Observer]")
{
    IT("Emits items pushed to onNext")
    {
        auto o = Observable<>::create<var>([](Observer<var> observer) {
            observer.onNext(3);
            observer.onNext("Hello");
        });

        Array<var> items;
        varxCollectItems(o, items);

        varxRequireItems(items, var(3), var("Hello"));
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
        CHECK(subject.getLatestItem() == "Initial Value");

        // Collect items from subject
        Array<var> items;
        varxCollectItems(subject, items);

        // Subscribe observer to some observable
        Observer<var> observer = subject;
        Observable<>::from<var>({ 3, 4, 5 }).subscribe(observer).disposedBy(disposeBag);

        // Subject should have received items from Observable
        varxRequireItems(items, var("Initial Value"), var(3), var(4), var(5));
        REQUIRE(subject.getLatestItem() == var(5));
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
            Array<var> items;
            Observer<float> o = vars;
            varxCollectItems(vars, items);
            o.onNext(14.57);
            
            varxRequireItems(items, 14.57f);
        }
        
        IT("can convert from float to var")
        {
            Array<float> items;
            Observer<var> o = floats;
            varxCollectItems(floats, items);
            o.onNext(var(85.2f));
            
            varxRequireItems(items, 85.2f);
        }
        
        IT("can convert from var to String")
        {
            Array<var> items;
            Observer<String> o = vars;
            varxCollectItems(vars, items);
            o.onNext("Hello world!");
            
            varxRequireItems(items, "Hello world!");
        }
        
        IT("can convert from float to int")
        {
            Array<int> items;
            Observer<float> o = ints;
            varxCollectItems(ints, items);
            o.onNext(15.67f);
            
            varxRequireItems(items, 15);
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
            bool operator!=(const Base& rhs) const { return !(*this == rhs); }
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
            Array<Base> items;
            Observer<Derived> o = bases;
            varxCollectItems(bases, items);
            o.onNext(Derived(205, 1002));
            
            varxRequireItems(items, Base(205));
        }
    }
}
