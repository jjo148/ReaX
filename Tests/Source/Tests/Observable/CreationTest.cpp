#include "../../Other/TestPrefix.h"

using Catch::Contains;


TEST_CASE("Observable::create",
          "[Observable][Observable::create]")
{
    Array<String> values;

    IT("emits values when pushing values synchronously")
    {
        auto observable = Observable<String>::create([](Observer<String> observer) {
            observer.onNext("First");
            observer.onNext("Second");
        });
        Reaction_CollectValues(observable, values);

        Reaction_RequireValues(values, "First", "Second");
    }

    IT("emits values when pushing values asynchronously")
    {
        auto observable = Observable<String>::create([](Observer<String> observer) {
            MessageManager::getInstance()->callAsync([observer]() {
                observer.onNext("First");
                observer.onNext("Second");
            });
        });
        Reaction_CollectValues(observable, values);

        // There shouldn't be any values until the async callback is executed
        CHECK(values.isEmpty());

        // The values should be there after running the dispatch loop
        Reaction_RunDispatchLoopUntil(values.size() == 2);
        Reaction_RequireValues(values, "First", "Second");
    }

    IT("emits can emit values asynchronously after being destroyed")
    {
        auto observable = std::make_shared<Observable<String>>(Observable<String>::create([](Observer<String> observer) {
            MessageManager::getInstance()->callAsync([observer]() {
                observer.onNext("First");
                observer.onNext("Second");
            });
        }));

        IT("emits when there's still a subscription")
        {
            auto subscription = observable->subscribe([&](String next) { values.add(next); });
            observable.reset();
            Reaction_RunDispatchLoopUntil(values.size() == 2);

            Reaction_RequireValues(values, "First", "Second");
        }

        IT("doesn't emit when the subscription has unsubscribed")
        {
            auto subscription = observable->subscribe([&](String next) { values.add(next); });
            observable.reset();
            subscription.unsubscribe();
            Reaction_RunDispatchLoop(20);

            REQUIRE(values.isEmpty());
        }
    }

    IT("calls onSubscribe again for each new subscription")
    {
        auto observable = Observable<String>::create([](Observer<String> observer) {
            observer.onNext("onSubscribe called");
        });
        Reaction_CollectValues(observable, values);
        Reaction_CollectValues(observable, values);
        Reaction_CollectValues(observable, values);

        Reaction_RequireValues(values, "onSubscribe called", "onSubscribe called", "onSubscribe called");
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
        auto observable = std::make_shared<Observable<String>>(Observable<String>::create([pointer](Observer<String>) {}));

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
    Array<int> values;

    IT("calls the factory function on every new subscription")
    {
        int numCalls = 0;
        auto observable = Observable<int>::defer([&]() {
            numCalls++;
            return Observable<int>::from({ 3, 4 });
        });

        Reaction_CollectValues(observable, values);
        Reaction_CollectValues(observable, values);
        Reaction_CollectValues(observable, values);

        Reaction_RequireValues(values, 3, 4, 3, 4, 3, 4);
        REQUIRE(numCalls == 3);
    }
}


TEST_CASE("Observable::empty",
          "[Observable][Observable::empty]")
{
    Array<int> values;
    auto o = Observable<int>::empty();

    IT("doesn't emit any values")
    {
        Reaction_CollectValues(o, values);
        Reaction_RunDispatchLoop(20);

        REQUIRE(values.isEmpty());
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
    Array<int> values;
    auto o = Observable<int>::error(std::runtime_error("Error!!111!"));
    DisposeBag disposeBag;

    IT("doesn't emit any values")
    {
        o.subscribe([&](var value) { values.add(value); }, [](std::exception_ptr) {}).disposedBy(disposeBag);
        Reaction_RunDispatchLoop(20);

        REQUIRE(values.isEmpty());
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
        Array<long> values;
        Reaction_CollectValues(Observable<int>::from(Array<int>({ 3, 6, 8 })), values);

        Reaction_RequireValues(values, 3, 6, 8);
    }

    IT("can be created from a std::initializer_list<var>")
    {
        Array<var> values;
        Reaction_CollectValues(Observable<var>::from({ var("Hello"), var(15.5) }), values);

        Reaction_RequireValues(values, var("Hello"), var(15.5));
    }

    IT("can be created from a std::initializer_list<int>")
    {
        Array<double> values;
        Reaction_CollectValues(Observable<int>::from({ 1, 4 }), values);

        Reaction_RequireValues(values, 1, 4);
    }

    IT("can be created from a std::initializer_list<const char*>")
    {
        Array<String> values;
        Reaction_CollectValues(Observable<String>::from({ "Hello", "Test" }), values);

        Reaction_RequireValues(values, "Hello", "Test");
    }
}


TEST_CASE("Observable::fromValue",
          "[Observable][Observable::fromValue]")
{
    Value value(String("Initial Value"));
    const auto observable = Observable<var>::fromValue(value);
    Array<String> values;
    Reaction_CollectValues(observable, values);

    Reaction_CheckValues(values, "Initial Value");

    IT("emits if a copy of the Value sets a new value")
    {
        Value copy(value);
        copy.setValue("Set by copy");
        Reaction_RunDispatchLoopUntil(values.size() == 2);

        Reaction_RequireValues(values, "Initial Value", "Set by copy");
    }

    IT("emites only one value if the Value is set multiple times synchronously")
    {
        value = "2";
        value = "3";
        value = "4";
        Reaction_RunDispatchLoopUntil(values.size() == 2);

        Reaction_RequireValues(values, "Initial Value", "4");
    }

    IT("notifies multiple Subscriptions on subscribe")
    {
        auto another = Observable<var>::fromValue(value);
        Reaction_CollectValues(another, values);

        Reaction_RequireValues(values, "Initial Value", "Initial Value");
    }

    IT("notifies multiple Values referring to the same ValueSource")
    {
        Value anotherValue(value);
        auto anotherObservable = Observable<var>::fromValue(anotherValue);
        Reaction_CollectValues(anotherObservable, values);

        Reaction_RequireValues(values, "Initial Value", "Initial Value");
    }

    IT("notifies multiple Subscriptions if a Value is set multiple times")
    {
        DisposeBag disposeBag;
        observable.subscribe([&](String newValue) {
                      values.add(newValue.toUpperCase());
                  })
            .disposedBy(disposeBag);

        value = "Bar";
        Reaction_RunDispatchLoopUntil(values.size() == 4);

        value = "Baz";
        Reaction_RunDispatchLoopUntil(values.size() == 6);

        CHECK(values.size() == 6);

        // Subscribers are notified in no particular order
        for (auto s : { "Initial Value", "INITIAL VALUE", "BAR", "Bar", "BAZ", "Baz" })
            REQUIRE(values.contains(s));
    }
}


TEST_CASE("Observable::fromValue lifetime",
          "[Observable][Observable::fromValue]")
{
    // Create an Observable from a Value
    Value value(String("Initial"));
    auto source = std::make_shared<Observable<var>>(Observable<var>::fromValue(value));

    // Create another Observable from the source Observable
    auto mapped = source->map([](String s) { return s; });

    // Collect values from the mapped Observable
    Array<var> values;
    Reaction_CollectValues(mapped, values);

    Reaction_CheckValues(values, "Initial");

    IT("emits values when the source Observable is alive")
    {
        value.setValue("New Value");
        Reaction_RunDispatchLoopUntil(values.size() == 2);

        Reaction_RequireValues(values, "Initial", "New Value");
    }

    IT("stops emitting values as soon as the source Observable is destroyed")
    {
        source.reset();
        value.setValue("Two");
        value.setValue("Three");
        Reaction_RunDispatchLoop(20);

        Reaction_RequireValues(values, "Initial");
    }

    IT("does not emit a value if the Observable is destroyed immediately after calling setValue")
    {
        value.setValue("New Value");
        source.reset();
        Reaction_RunDispatchLoop(20);

        Reaction_RequireValues(values, "Initial");
    }

    IT("continues to emit values if the source Observable is copied and then destroyed")
    {
        auto copy = std::make_shared<Observable<var>>(*source);
        Array<var> copyValues;
        Reaction_CollectValues(*copy, copyValues);

        Reaction_CheckValues(copyValues, "Initial");

        source.reset();
        Reaction_RunDispatchLoop(20);
        value.setValue("New");
        Reaction_RunDispatchLoopUntil(values.size() == 2);

        Reaction_RequireValues(copyValues, "Initial", "New");
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
    auto o = Observable<var>::fromValue(slider.getValueObject());
    Array<var> values;
    Reaction_CollectValues(o, values);
    Reaction_CheckValues(values, 7.6);

    IT("emits once if the Slider is changed once")
    {
        slider.setValue(0.45);
        Reaction_RunDispatchLoopUntil(values.size() == 2);

        Reaction_RequireValues(values, 7.6, 0.45);
    }

    IT("emits just once if the Slider value changes rapidly")
    {
        for (double value : { 3.41, 9.54, 4.67, 3.56 })
            slider.setValue(value);

        Reaction_RunDispatchLoopUntil(values.size() == 2);

        Reaction_RequireValues(values, 7.6, 3.56);
    }
}


TEST_CASE("Observable::interval",
          "[Observable][Observable::interval]")
{
    IT("can create an interval below one second")
    {
        auto o = Observable<int>::interval(RelativeTime::seconds(0.04)).take(3);
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

        Reaction_RequireValues(ints, 1, 2, 3);
    }
}


TEST_CASE("Observable::just",
          "[Observable][Observable::just]")
{
    IT("emits a single value on subscribe")
    {
        Array<float> values;
        Reaction_CollectValues(Observable<double>::just(18.3), values);

        Reaction_RequireValues(values, 18.3);
    }

    IT("notifies multiple subscriptions")
    {
        Array<String> values;
        auto o = Observable<String>::just("Hello");
        Reaction_CollectValues(o, values);
        Reaction_CollectValues(o, values);

        Reaction_RequireValues(values, "Hello", "Hello");
    }
}


TEST_CASE("Observable::never",
          "[Observable][Observable::never]")
{
    auto o = Observable<int64>::never();
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

        Reaction_RunDispatchLoop(20);

        REQUIRE(!onNextCalled);
        REQUIRE(!onErrorCalled);
        REQUIRE(!onCompletedCalled);
    }
}


TEST_CASE("Observable::range",
          "[Observable][Observable::range]")
{
    Array<var> values;

    IT("emits integer numbers with an integer range")
    {
        Reaction_CollectValues(Observable<int>::range(3, 7, 3), values);
        Reaction_RequireValues(values, 3, 6, 7);
    }

    IT("emits double numbers with a double range")
    {
        Reaction_CollectValues(Observable<double>::range(17.5, 22.8, 2), values);
        Reaction_RequireValues(values, 17.5, 19.5, 21.5, 22.8);
    }

    IT("emits just start if start == end")
    {
        Reaction_CollectValues(Observable<int>::range(10, 10), values);
        Reaction_RequireValues(values, 10);
    }

    IT("throws if start > end")
    {
        REQUIRE_THROWS_WITH(Observable<int>::range(10, 9), Contains("Invalid range"));
    }
}


TEST_CASE("Observable::repeat",
          "[Observable][Observable::repeat]")
{
    Array<var> values;

    IT("repeats a value indefinitely")
    {
        Reaction_CollectValues(Observable<int>::repeat(8).take(9), values);

        Reaction_RequireValues(values, 8, 8, 8, 8, 8, 8, 8, 8, 8);
    }

    IT("repeats a values a limited number of times")
    {
        Reaction_CollectValues(Observable<String>::repeat("4", 7), values);

        Reaction_RequireValues(values, "4", "4", "4", "4", "4", "4", "4");
    }
}

TEST_CASE("Observable covariance",
          "[Observable]")
{
    CONTEXT("implicit conversion")
    {
        auto floats = Observable<float>::just(17.f);
        auto doubles = Observable<double>::just(34.0);
        auto vars = Observable<var>::just(51);
        auto strings = Observable<String>::just("Hello");
        
        IT("can convert from var to float")
        {
            Array<float> values;
            floats = vars;
            Reaction_CollectValues(floats, values);
            
            Reaction_RequireValues(values, 51);
        }
        
        IT("can convert from float to var")
        {
            Array<var> values;
            vars = floats;
            Reaction_CollectValues(vars, values);
            
            Reaction_RequireValues(values, 17);
        }
        
        IT("can convert from String to var")
        {
            Array<var> values;
            vars = strings;
            Reaction_CollectValues(vars, values);
            
            Reaction_RequireValues(values, "Hello");
        }
        
        IT("can convert from float to double")
        {
            Array<double> values;
            doubles = floats;
            Reaction_CollectValues(doubles, values);
            
            Reaction_RequireValues(values, 17);
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
        
        auto bases = Observable<Base>::just(Base(100));
        auto deriveds = Observable<Derived>::just(Derived(200, 1000));
        
        IT("can convert from Deriveds to Bases")
        {
            Array<Base> values;
            bases = deriveds;
            Reaction_CollectValues(bases, values);
            
            Reaction_RequireValues(values, Base(200));
        }
    }
}
