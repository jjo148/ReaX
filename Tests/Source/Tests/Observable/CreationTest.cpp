#include "../../Other/TestPrefix.h"

using Catch::Contains;


TEST_CASE("Observable::create",
          "[Observable][Observable::create]")
{
    Array<String> items;

    IT("emits items when pushing items synchronously")
    {
        auto observable = Observable<>::create<String>([](Observer<String> observer) {
            observer.onNext("First");
            observer.onNext("Second");
        });
        varxCollectItems(observable, items);

        varxRequireItems(items, "First", "Second");
    }

    IT("emits items when pushing items asynchronously")
    {
        auto observable = Observable<>::create<String>([](Observer<String> observer) {
            MessageManager::getInstance()->callAsync([observer]() {
                observer.onNext("First");
                observer.onNext("Second");
            });
        });
        varxCollectItems(observable, items);

        // There shouldn't be any items until the async callback is executed
        CHECK(items.isEmpty());

        // The items should be there after running the dispatch loop
        varxRunDispatchLoopUntil(items.size() == 2);
        varxRequireItems(items, "First", "Second");
    }

    IT("emits can emit items asynchronously after being destroyed")
    {
        auto observable = std::make_shared<Observable<String>>(Observable<>::create<String>([](Observer<String> observer) {
            MessageManager::getInstance()->callAsync([observer]() {
                observer.onNext("First");
                observer.onNext("Second");
            });
        }));

        IT("emits when there's still a subscription")
        {
            auto subscription = observable->subscribe([&](String next) { items.add(next); });
            observable.reset();
            varxRunDispatchLoopUntil(items.size() == 2);

            varxRequireItems(items, "First", "Second");
        }

        IT("doesn't emit when the subscription has unsubscribed")
        {
            auto subscription = observable->subscribe([&](String next) { items.add(next); });
            observable.reset();
            subscription.unsubscribe();
            varxRunDispatchLoop(20);

            REQUIRE(items.isEmpty());
        }
    }

    IT("calls onSubscribe again for each new subscription")
    {
        auto observable = Observable<>::create<String>([](Observer<String> observer) {
            observer.onNext("onSubscribe called");
        });
        varxCollectItems(observable, items);
        varxCollectItems(observable, items);
        varxCollectItems(observable, items);

        varxRequireItems(items, "onSubscribe called", "onSubscribe called", "onSubscribe called");
    }

    IT("captures an object until the Observable is destroyed")
    {
        // Create a ref counted object
        class Dummy : public ReferenceCountedObject
        {
        public:
            Dummy()
            : ReferenceCountedObject() {}
        };
        ReferenceCountedObjectPtr<ReferenceCountedObject> pointer(new Dummy());

        // Capture it in the Observable
        auto observable = std::make_shared<Observable<String>>(Observable<>::create<String>([pointer](Observer<String>) {}));

        // There should be 2 references: From pointer and from the Observable
        CHECK(pointer->getReferenceCount() == 2);

        // If a copy of the Observable is made, it should still be 2
        auto copy = std::make_shared<Observable<String>>(*observable);
        CHECK(pointer->getReferenceCount() == 2);

        // After the first Observable is destroyed, there should still be 2
        observable.reset();
        CHECK(pointer->getReferenceCount() == 2);

        // Creating a copy should not increase the ref count
        DisposeBag disposeBag;
        copy->subscribe([](var) {}).disposedBy(disposeBag);
        CHECK(pointer->getReferenceCount() == 2);

        // After the copy is destroyed, there should be just 1 (from the pointer)
        copy.reset();
        REQUIRE(pointer->getReferenceCount() == 1);
    }
}


TEST_CASE("Observable::defer",
          "[Observable][Observable::defer]")
{
    Array<int> items;

    IT("calls the factory function on every new subscription")
    {
        int numCalls = 0;
        auto observable = Observable<>::defer<int>([&]() {
            numCalls++;
            return Observable<>::from<int>({ 3, 4 });
        });

        varxCollectItems(observable, items);
        varxCollectItems(observable, items);
        varxCollectItems(observable, items);

        varxRequireItems(items, 3, 4, 3, 4, 3, 4);
        REQUIRE(numCalls == 3);
    }
}


TEST_CASE("Observable::empty",
          "[Observable][Observable::empty]")
{
    Array<int> items;
    auto o = Observable<>::empty<int>();

    IT("doesn't emit any items")
    {
        varxCollectItems(o, items);
        varxRunDispatchLoop(20);

        REQUIRE(items.isEmpty());
    }

    IT("notifies onCompleted immediately")
    {
        DisposeBag disposeBag;
        bool completed = false;
        o.subscribe([](var) {}, [](std::exception_ptr) {}, [&]() { completed = true; }).disposedBy(disposeBag);

        REQUIRE(completed);
    }
}


TEST_CASE("Observable::error",
          "[Observable][Observable::error]")
{
    Array<int> items;
    auto o = Observable<>::error<int>(std::runtime_error("Error!!111!"));
    DisposeBag disposeBag;

    IT("doesn't emit any items")
    {
        o.subscribe([&](var item) { items.add(item); }, [](std::exception_ptr) {}).disposedBy(disposeBag);
        varxRunDispatchLoop(20);

        REQUIRE(items.isEmpty());
    }

    IT("notifies onCompleted immediately")
    {
        bool onErrorCalled = false;
        o.subscribe([](var) {}, [&](std::exception_ptr) { onErrorCalled = true; }).disposedBy(disposeBag);

        REQUIRE(onErrorCalled);
    }
}


TEST_CASE("Observable::from",
          "[Observable][Observable::from]")
{
    IT("can be created from an Array<int>")
    {
        Array<long> items;
        varxCollectItems(Observable<>::from<int>(Array<int>({ 3, 6, 8 })), items);

        varxRequireItems(items, 3, 6, 8);
    }

    IT("can be created from a std::initializer_list<var>")
    {
        Array<var> items;
        varxCollectItems(Observable<>::from<var>({ var("Hello"), var(15.5) }), items);

        varxRequireItems(items, var("Hello"), var(15.5));
    }

    IT("can be created from a std::initializer_list<int>")
    {
        Array<double> items;
        varxCollectItems(Observable<>::from<int>({ 1, 4 }), items);

        varxRequireItems(items, 1, 4);
    }

    IT("can be created from a std::initializer_list<String>")
    {
        Array<String> items;
        varxCollectItems(Observable<>::from<String>({ "Hello", "Test" }), items);

        varxRequireItems(items, "Hello", "Test");
    }
}


TEST_CASE("Observable::fromValue",
          "[Observable][Observable::fromValue]")
{
    Value value(String("Initial Item"));
    const auto observable = Observable<>::fromValue(value);
    Array<String> items;
    varxCollectItems(observable, items);

    varxCheckItems(items, "Initial Item");

    IT("emits if a copy of the Value sets a new value")
    {
        Value copy(value);
        copy.setValue("Set by copy");
        varxRunDispatchLoopUntil(items.size() == 2);

        varxRequireItems(items, "Initial Item", "Set by copy");
    }

    IT("emites only one item if the Value is set multiple times synchronously")
    {
        value = "2";
        value = "3";
        value = "4";
        varxRunDispatchLoopUntil(items.size() == 2);

        varxRequireItems(items, "Initial Item", "4");
    }

    IT("notifies multiple Subscriptions on subscribe")
    {
        auto another = Observable<>::fromValue(value);
        varxCollectItems(another, items);

        varxRequireItems(items, "Initial Item", "Initial Item");
    }

    IT("notifies multiple Values referring to the same ValueSource")
    {
        Value anotherValue(value);
        auto anotherObservable = Observable<>::fromValue(anotherValue);
        varxCollectItems(anotherObservable, items);

        varxRequireItems(items, "Initial Item", "Initial Item");
    }

    IT("notifies multiple Subscriptions if a Value is set multiple times")
    {
        DisposeBag disposeBag;
        observable.subscribe([&](String newValue) {
                      items.add(newValue.toUpperCase());
                  })
            .disposedBy(disposeBag);

        value = "Bar";
        varxRunDispatchLoopUntil(items.size() == 4);

        value = "Baz";
        varxRunDispatchLoopUntil(items.size() == 6);

        CHECK(items.size() == 6);

        // Subscribers are notified in no particular order
        for (auto s : { "Initial Item", "INITIAL ITEM", "BAR", "Bar", "BAZ", "Baz" })
            REQUIRE(items.contains(s));
    }
}


TEST_CASE("Observable::fromValue lifetime",
          "[Observable][Observable::fromValue]")
{
    // Create an Observable from a Value
    Value value(String("Initial"));
    auto source = std::make_shared<Observable<var>>(Observable<>::fromValue(value));

    // Create another Observable from the source Observable
    auto mapped = source->map([](String s) { return s; });

    // Collect items from the mapped Observable
    Array<var> items;
    varxCollectItems(mapped, items);

    varxCheckItems(items, "Initial");

    IT("emits items when the source Observable is alive")
    {
        value.setValue("New Value");
        varxRunDispatchLoopUntil(items.size() == 2);

        varxRequireItems(items, "Initial", "New Value");
    }

    IT("stops emitting items as soon as the source Observable is destroyed")
    {
        source.reset();
        value.setValue("Two");
        value.setValue("Three");
        varxRunDispatchLoop(20);

        varxRequireItems(items, "Initial");
    }

    IT("does not emit an item if the Observable is destroyed immediately after calling setValue")
    {
        value.setValue("New Value");
        source.reset();
        varxRunDispatchLoop(20);

        varxRequireItems(items, "Initial");
    }

    IT("continues to emit items if the source Observable is copied and then destroyed")
    {
        auto copy = std::make_shared<Observable<var>>(*source);
        Array<var> copyItems;
        varxCollectItems(*copy, copyItems);

        varxCheckItems(copyItems, "Initial");

        source.reset();
        varxRunDispatchLoop(20);
        value.setValue("New");
        varxRunDispatchLoopUntil(items.size() == 2);

        varxRequireItems(copyItems, "Initial", "New");
    }

    IT("notified onComplete when the Observable is destroyed")
    {
        bool completed = false;
        source->subscribe([](var) {}, [](std::exception_ptr) {}, [&]() { completed = true; });
        CHECK(!completed);

        source.reset();

        REQUIRE(completed);
    }
}


TEST_CASE("Observable::fromValue with a Slider",
          "[Observable][Observable::fromValue]")
{
    Slider slider;
    slider.setValue(7.6);
    auto o = Observable<>::fromValue(slider.getValueObject());
    Array<var> items;
    varxCollectItems(o, items);
    varxCheckItems(items, 7.6);

    IT("emits once if the Slider is changed once")
    {
        slider.setValue(0.45);
        varxRunDispatchLoopUntil(items.size() == 2);

        varxRequireItems(items, 7.6, 0.45);
    }

    IT("emits just once if the Slider value changes rapidly")
    {
        for (double value : { 3.41, 9.54, 4.67, 3.56 })
            slider.setValue(value);

        varxRunDispatchLoopUntil(items.size() == 2);

        varxRequireItems(items, 7.6, 3.56);
    }
}


TEST_CASE("Observable::interval",
          "[Observable][Observable::interval]")
{
    IT("can create an interval below one second")
    {
        auto o = Observable<>::interval(RelativeTime::seconds(0.04)).take(3);
        auto lastTime = Time::getCurrentTime();
        Array<RelativeTime> intervals;
        Array<int> ints;
        o.subscribe([&](int i) {
            auto time = Time::getCurrentTime();
            intervals.add(time - lastTime);
            lastTime = time;
            ints.add(i);
        });

        CHECK(intervals.size() == 3);
        REQUIRE(intervals[0].inSeconds() == Approx(0).epsilon(0.03));
        REQUIRE(intervals[1].inSeconds() == Approx(0.04).epsilon(0.03));
        REQUIRE(intervals[2].inSeconds() == Approx(0.04).epsilon(0.03));

        varxRequireItems(ints, 1, 2, 3);
    }
}


TEST_CASE("Observable::just",
          "[Observable][Observable::just]")
{
    IT("emits a single value on subscribe")
    {
        Array<float> items;
        varxCollectItems(Observable<>::just(18.3), items);

        varxRequireItems(items, 18.3);
    }

    IT("notifies multiple subscriptions")
    {
        Array<String> items;
        auto o = Observable<>::just<String>("Hello");
        varxCollectItems(o, items);
        varxCollectItems(o, items);

        varxRequireItems(items, "Hello", "Hello");
    }
}


TEST_CASE("Observable::never",
          "[Observable][Observable::never]")
{
    auto o = Observable<>::never<int64>();
    DisposeBag disposeBag;

    IT("doesn't terminate and doesn't emit")
    {
        bool onNextCalled = false;
        bool onErrorCalled = false;
        bool onCompletedCalled = false;
        o.subscribe([&](int64) { onNextCalled = true; },
                    [&](std::exception_ptr) { onErrorCalled = true; },
                    [&]() { onCompletedCalled = true; })
            .disposedBy(disposeBag);

        varxRunDispatchLoop(20);

        REQUIRE(!onNextCalled);
        REQUIRE(!onErrorCalled);
        REQUIRE(!onCompletedCalled);
    }
}


TEST_CASE("Observable::range",
          "[Observable][Observable::range]")
{
    Array<var> items;

    IT("emits integer numbers with an integer range")
    {
        varxCollectItems(Observable<>::range(3, 7, 3), items);
        varxRequireItems(items, 3, 6, 7);
    }

    IT("emits double numbers with a double range")
    {
        varxCollectItems(Observable<>::range(17.5, 22.8, 2), items);
        varxRequireItems(items, 17.5, 19.5, 21.5, 22.8);
    }

    IT("emits just start if start == end")
    {
        varxCollectItems(Observable<>::range(10, 10), items);
        varxRequireItems(items, 10);
    }

    IT("throws if start > end")
    {
        REQUIRE_THROWS_WITH(Observable<>::range(10, 9), Contains("Invalid range"));
    }
}


TEST_CASE("Observable::repeat",
          "[Observable][Observable::repeat]")
{
    Array<var> items;

    IT("repeats an item indefinitely")
    {
        varxCollectItems(Observable<>::repeat(8).take(9), items);

        varxRequireItems(items, 8, 8, 8, 8, 8, 8, 8, 8, 8);
    }

    IT("repeats an items a limited number of times")
    {
        varxCollectItems(Observable<>::repeat<String>("4", 7), items);

        varxRequireItems(items, "4", "4", "4", "4", "4", "4", "4");
    }
}

TEST_CASE("Observable covariance",
          "[Observable]")
{
    CONTEXT("implicit conversion")
    {
        auto floats = Observable<>::just(17.f);
        auto doubles = Observable<>::just(34.0);
        auto vars = Observable<>::just<var>(51);
        auto strings = Observable<>::just<String>("Hello");
        
        IT("can convert from var to float")
        {
            Array<float> items;
            floats = vars;
            varxCollectItems(floats, items);
            
            varxRequireItems(items, 51);
        }
        
        IT("can convert from float to var")
        {
            Array<var> items;
            vars = floats;
            varxCollectItems(vars, items);
            
            varxRequireItems(items, 17);
        }
        
        IT("can convert from String to var")
        {
            Array<var> items;
            vars = strings;
            varxCollectItems(vars, items);
            
            varxRequireItems(items, "Hello");
        }
        
        IT("can convert from float to double")
        {
            Array<double> items;
            doubles = floats;
            varxCollectItems(doubles, items);
            
            varxRequireItems(items, 17);
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
        
        auto bases = Observable<>::just(Base(100));
        auto deriveds = Observable<>::just(Derived(200, 1000));
        
        IT("can convert from Deriveds to Bases")
        {
            Array<Base> items;
            bases = deriveds;
            varxCollectItems(bases, items);
            
            varxRequireItems(items, Base(200));
        }
    }
}
