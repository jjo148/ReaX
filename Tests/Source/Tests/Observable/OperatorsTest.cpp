#include "../../Other/TestPrefix.h"


template<typename T, typename... Ts>
String concatStrings(T t, Ts... ts)
{
    return t + concatStrings(ts...);
}

template<>
String concatStrings<String>(String s)
{
    return s;
}

TEST_CASE("Observable::combineLatest",
          "[Observable][Observable::combineLatest]")
{
    Array<String> values;
    OwnedArray<Observable<String>> os;
    for (int i = 0; i < 8; i++)
        os.add(new Observable<String>(Observable<String>::just(String(i) + " ")));

    IT("works with arity 1")
    {
        const auto f = concatStrings<String, String>;
        Reaction_CollectValues(os[0]->combineLatest(f, *os[1]), values);
        Reaction_RequireValues(values, "0 1 ");
    }

    IT("works with arity 2")
    {
        const auto f = concatStrings<String, String, String>;
        Reaction_CollectValues(os[0]->combineLatest(f, *os[1], *os[2]), values);
        Reaction_RequireValues(values, "0 1 2 ");
    }

    IT("works with arity 3")
    {
        const auto f = concatStrings<String, String, String, String>;
        Reaction_CollectValues(os[0]->combineLatest(f, *os[1], *os[2], *os[3]), values);
        Reaction_RequireValues(values, "0 1 2 3 ");
    }

    IT("works with arity 4")
    {
        const auto f = concatStrings<String, String, String, String, String>;
        Reaction_CollectValues(os[0]->combineLatest(f, *os[1], *os[2], *os[3], *os[4]), values);
        Reaction_RequireValues(values, "0 1 2 3 4 ");
    }

    IT("works with arity 5")
    {
        const auto f = concatStrings<String, String, String, String, String, String>;
        Reaction_CollectValues(os[0]->combineLatest(f, *os[1], *os[2], *os[3], *os[4], *os[5]), values);
        Reaction_RequireValues(values, "0 1 2 3 4 5 ");
    }

    IT("works with arity 6")
    {
        const auto f = concatStrings<String, String, String, String, String, String, String>;
        Reaction_CollectValues(os[0]->combineLatest(f, *os[1], *os[2], *os[3], *os[4], *os[5], *os[6]), values);
        Reaction_RequireValues(values, "0 1 2 3 4 5 6 ");
    }

    IT("works with arity 7")
    {
        const auto f = concatStrings<String, String, String, String, String, String, String, String>;
        Reaction_CollectValues(os[0]->combineLatest(f, *os[1], *os[2], *os[3], *os[4], *os[5], *os[6], *os[7]), values);
        Reaction_RequireValues(values, "0 1 2 3 4 5 6 7 ");
    }

    IT("combines elements into a tuple when no function is given")
    {
        auto o1 = Observable<bool>::just(true);
        auto o2 = Observable<String>::from({ "Hello", "World" });
        auto o3 = Observable<long long>::range(4, 6);
        auto combined = o1.combineLatest(o2, o3);
        static_assert(std::is_same<decltype(combined), Observable<std::tuple<bool, String, long long>>>::value, "Combined Observable has wrong type.");

        Array<std::tuple<bool, String, int>> values;
        Reaction_CollectValues(combined, values);
        
        CHECK(values.size() == 3);
        
        Reaction_RequireValues(values,
                         std::make_tuple(true, "World", 4),
                         std::make_tuple(true, "World", 5),
                         std::make_tuple(true, "World", 6));
    }
}


TEST_CASE("Observable::concat",
          "[Observable][Observable::concat]")
{
    Array<var> values;

    IT("concatenates the values emitted by the source Observables")
    {
        auto observable = Observable<var>::from({ "Hello", "World" });
        auto another = Observable<var>::from({ 1.5, 2.32, 5.6 });
        Reaction_CollectValues(observable.concat({another}), values);

        Reaction_RequireValues(values, var("Hello"), var("World"), var(1.5), var(2.32), var(5.6));
    }
}


TEST_CASE("Observable::distinctUntilChanged",
          "[Observable][Observable::distinctUntilChanged]")
{
    IT("doesn't emit consecutive duplicate integers")
    {
        Array<var> originalValues;
        Array<var> filteredValues;
        PublishSubject<var> subject;

        Reaction_CollectValues(subject, originalValues);
        Reaction_CollectValues(subject.distinctUntilChanged(), filteredValues);

        subject.onNext(3);
        subject.onNext(3);
        subject.onNext("3"); // Is equal to 3, due to how juce::var defines operator==
        subject.onNext(3);
        subject.onNext(3);
        subject.onNext(5);
        subject.onNext(3);

        Reaction_RequireValues(originalValues, var(3), var(3), var("3"), var(3), var(3), var(5), var(3));
        Reaction_RequireValues(filteredValues, var(3), var(5), var(3));
    }

    IT("doesn't emit consecutive duplicate Point<int>s")
    {
        Array<Point<int>> values;
        PublishSubject<Point<int>> subject;

        // Declare Point<int> as the type parameter, to use Point<int>::operator== for comparison:
        Reaction_CollectValues(subject.distinctUntilChanged(), values);

        subject.onNext(Point<int>(27, 12));
        subject.onNext(Point<int>(27, 12));
        subject.onNext(Point<int>(27, 14));

        Reaction_RequireValues(values, Point<int>(27, 12), Point<int>(27, 14));
    }
}


TEST_CASE("Observable::elementAt",
          "[Observable][Observable::elementAt]")
{
    auto observable = Observable<float>::from({ 17.4, 3.0, 1.5, 6.77 });
    Array<double> values;

    IT("emits only the value at the given index")
    {
        Reaction_CollectValues(observable.elementAt(2), values);

        Reaction_RequireValues(values, 1.5);
    }
}


TEST_CASE("Observable::filter",
          "[Observable][Observable::filter]")
{
    IT("filters ints")
    {
        Array<int> values;
        auto source = Observable<int>::range(4, 9, 1);
        auto filtered = source.filter([](int i) {
            return (i % 2 == 0);
        });
        Reaction_CollectValues(filtered, values);

        Reaction_RequireValues(values, 4, 6, 8);
    }

    IT("filters Strings")
    {
        Array<String> values;
        auto source = Observable<String>::from({ "Hello", "Great", "World", "Hey" });
        auto filtered = source.filter([](String s) {
            return s[0] == 'H';
        });
        Reaction_CollectValues(filtered, values);

        Reaction_RequireValues(values, "Hello", "Hey");
    }

    IT("filters an Observable which emits different types")
    {
        Array<var> values;
        auto source = Observable<var>::from({ 3, "Hello", 5.43 });
        auto filtered = source.filter([](var v) {
            return v.isDouble();
        });
        Reaction_CollectValues(filtered, values);

        Reaction_RequireValues(values, 5.43);
    }

    IT("works with std::bind")
    {
        Array<int> values;
        auto source = Observable<int>::range(14, 19);

        struct Test
        {
            bool test(int value) { return value < 17; }
        };

        Test t;
        auto predicate = std::bind(&Test::test, &t, std::placeholders::_1);
        auto filtered = source.filter(predicate);

        Reaction_CollectValues(filtered, values);
        Reaction_RequireValues(values, 14, 15, 16);
    }
}


TEST_CASE("Observable::flatMap",
          "[Observable][Observable::flatMap]")
{
    Array<String> values;

    IT("merges the values emitted by the returned Observables")
    {
        auto o = Observable<String>::from({ "Hello", "World" }).flatMap([](String s) {
            return Observable<String>::from({ s.toLowerCase(), s.toUpperCase() + "!" });
        });
        Reaction_CollectValues(o, values);

        Reaction_RequireValues(values, "hello", "HELLO!", "world", "WORLD!");
    }
}


TEST_CASE("Observable::map",
          "[Observable][Observable::map]")
{
    Array<long long> values;
    auto source = Observable<long>::range(4, 7, 2);

    IT("emits values synchronously")
    {
        auto mapped = source.map([](long l) { return l * 1.5; });
        Reaction_CollectValues(mapped, values);

        Reaction_RequireValues(values, 6.0, 9.0, 10.5);
    }
}


TEST_CASE("Interaction between Observable::map and Observable::switchOnNext",
          "[Observable][Observable::map][Observable::switchOnNext]")
{
    IT("supports returning Observables in map, even nested twice")
    {
        Array<String> values;
        auto source = Observable<int>::just(1);
        auto nested = source.map([](int i) {
            return Observable<String>::just("Hello").map([i](String s) {
                return Observable<var>::just(String(i) + " " + s);
            });
        });

        // Unwrap twice
        auto unwrapped = nested.switchOnNext().switchOnNext();
        Reaction_CollectValues(unwrapped, values);

        Reaction_RequireValues(values, "1 Hello");
    }

    IT("continues to emit values after the source Observable is gone")
    {
        Array<int> values;
        auto source = std::make_shared<Observable<int>>(Observable<int>::just(17));
        auto mapped = source->map([](int next) {
            return Observable<int>::create([next](Observer<int> observer) {
                MessageManager::getInstance()->callAsync([observer, next]() {
                    observer.onNext(next * 3);
                });
            });
        });
        auto unwrapped = mapped.switchOnNext();
        Reaction_CollectValues(unwrapped, values);

        // There should be no values before running dispatch loop
        CHECK(values.isEmpty());

        source.reset();
        Reaction_RunDispatchLoopUntil(!values.isEmpty());

        // The value should be emitted, although there's no reference to the source anymore
        Reaction_RequireValues(values, 17 * 3);
    }
}


TEST_CASE("Observable::merge",
          "[Observable][Observable::merge]")
{
    Array<int> values;
    
    IT("works with arity 8")
    {
        Array<Observable<int>> os;
        
        for (int i = 0; i < 8; ++i)
            os.add(Observable<int>::range(-i, 1));
        
        auto merged = os[0].merge({os[1], os[2], os[3], os[4], os[5], os[6], os[7]});
        Reaction_CollectValues(merged, values);
        
        CHECK(values.size() == 44);
        Reaction_RequireValues(values, 0, 1, -1, 0, 1, -2, -1, 0, 1, -3, -2, -1, 0, 1, -4, -3, -2, -1, 0, 1, -5, -4, -3, -2, -1, 0, 1, -6, -5, -4, -3, -2, -1, 0, 1, -7, -6, -5, -4, -3, -2, -1, 0, 1);
    }
}


TEST_CASE("Observable::reduce",
          "[Observable][Observable::reduce]")
{
    Array<int> values;

    IT("reduces emitted values")
    {
        auto observable = Observable<int>::from({ 10, 100, 1000 }).reduce(2, [](int accum, int next) {
            return accum + next;
        });

        Reaction_CollectValues(observable, values);

        Reaction_RequireValues(values, 1112);
    }
}


TEST_CASE("Observable::scan",
          "[Observable][Observable::scan]")
{
    Array<int> values;

    IT("applies the function to the inputs")
    {
        auto o = Observable<int>::range(1, 5).scan(10, [](int accum, int currentValue) {
            return accum + currentValue;
        });
        Reaction_CollectValues(o, values);

        Reaction_RequireValues(values, 11, 13, 16, 20, 25);
    }
}


TEST_CASE("Observable::skip",
          "[Observable][Observable::skip]")
{
    Array<int> values;

    IT("skips the first 4 values")
    {
        auto o = Observable<int>::from({ 4, 7, 2, 1, 19, 1, 33, 4 }).skip(4);
        Reaction_CollectValues(o, values);

        Reaction_RequireValues(values, 19, 1, 33, 4);
    }
}


TEST_CASE("Observable::skipUntil",
          "[Observable][Observable::skipUntil]")
{
    Array<String> values;

    IT("skips until another Observable emits a value")
    {
        PublishSubject<String> subject;
        PublishSubject<var> trigger;

        Reaction_CollectValues(subject.skipUntil(trigger), values);

        // Emit some values, these should NOT be received
        subject.onNext("Not");
        subject.onNext("Getting");
        subject.onNext("This");

        // Trigger
        trigger.onNext(var::undefined());

        // Emit more values, these should be received
        subject.onNext("These");
        subject.onNext("Are");
        subject.onNext("Received");

        Reaction_RequireValues(values, "These", "Are", "Received");
    }
}


TEST_CASE("Observable::startWith",
          "[Observable][Observable::startWith]")
{
    Array<int> values;
    auto observable = Observable<int>::from({ 17, 3 });

    IT("prepends values to an existing Observable")
    {
        Reaction_CollectValues(observable.startWith({6, 4, 7, 2}), values);

        Reaction_RequireValues(values, 6, 4, 7, 2, 17, 3);
    }
}


TEST_CASE("Observable::takeLast",
          "[Observable][Observable::takeLast]")
{
    Array<String> values;
    auto observable = Observable<String>::from({ "First", "Another", "And one more", "Last value" });

    IT("takes the last 2 emitted values")
    {
        Reaction_CollectValues(observable.takeLast(2), values);

        Reaction_RequireValues(values, "And one more", "Last value");
    }
}


TEST_CASE("Observable::takeUntil",
          "[Observable][Observable::takeUntil]")
{
    Array<String> values;

    IT("emits until another Observable emits a value")
    {
        PublishSubject<String> subject;
        PublishSubject<String> trigger;

        Reaction_CollectValues(subject.takeUntil(trigger), values);

        // Emit some values, these should be received
        subject.onNext("These");
        subject.onNext("Are");
        subject.onNext("Received");

        // Trigger
        trigger.onNext("Hey stop!");

        // Emit more values, these should NOT be received
        subject.onNext("Not");
        subject.onNext("Getting");
        subject.onNext("This");

        Reaction_RequireValues(values, "These", "Are", "Received");
    }
}


TEST_CASE("Observable::takeWhile",
          "[Observable][Observable::takeWhile]")
{
    Array<int> values;

    IT("emits values as long as the predicate returns true")
    {
        PublishSubject<int> subject;
        const auto predicate = [](int i) {
            return i <= 10;
        };

        Reaction_CollectValues(subject.takeWhile(predicate), values);

        // These should be emitted
        subject.onNext(4);
        subject.onNext(7);
        subject.onNext(10);

        // These shouldn't be emitted, because predicate(11) == false
        subject.onNext(11);
        subject.onNext(3);
        subject.onNext(7);

        Reaction_RequireValues(values, 4, 7, 10);
    }
}


TEST_CASE("Observable::withLatestFrom",
          "[Observable][Observable::withLatestFrom]")
{
    Array<String> values;
    PublishSubject<String> s1;
    PublishSubject<String> s2;

    IT("only emits when the first Observable emits")
    {
        const auto f = concatStrings<String, String>;
        Reaction_CollectValues(s1.withLatestFrom(f, s2), values);
        CHECK(values.isEmpty());
        s2.onNext("World!");
        CHECK(values.isEmpty());
        s1.onNext("Hello ");

        Reaction_RequireValues(values, "Hello World!");
    }
}


TEST_CASE("Observable::zip",
          "[Observable][Observable::zip]")
{
    Array<String> values;

    IT("zips three Observables together")
    {
        PublishSubject<String> strings;
        PublishSubject<int> ints;
        PublishSubject<double> doubles;
        const auto combine = [](String s, int i, double d) {
            return "s=" + s + "; i=" + String(i) + "; d=" + String(d);
        };

        Reaction_CollectValues(strings.zip(combine, ints, doubles), values);

        // First value should be emitted when all three Observables have emitted once
        strings.onNext("a");
        CHECK(values.isEmpty());
        ints.onNext(1);
        CHECK(values.isEmpty());
        doubles.onNext(0.1);
        Reaction_CheckValues(values, "s=a; i=1; d=0.1");

        // Second value should be emitted when all three Observables have emitted twice
        doubles.onNext(0.25);
        CHECK(values.size() == 1);
        ints.onNext(57);
        CHECK(values.size() == 1);
        strings.onNext("x");
        Reaction_RequireValues(values, "s=a; i=1; d=0.1", "s=x; i=57; d=0.25");
    }
}
