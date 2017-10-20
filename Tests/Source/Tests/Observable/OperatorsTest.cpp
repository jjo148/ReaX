#include "../../Other/TestPrefix.h"


template<typename T, typename... Ts>
String transform(T t, Ts... ts)
{
    return t + transform(ts...);
}

template<>
String transform<String>(String s)
{
    return s;
}

TEST_CASE("Observable::combineLatest",
          "[Observable][Observable::combineLatest]")
{
    Array<String> items;
    OwnedArray<Observable<String>> os;
    for (int i = 0; i < 8; i++)
        os.add(new Observable<String>(Observable<String>::just(String(i) + " ")));

    IT("works with arity 1")
    {
        const auto f = transform<String, String>;
        varxCollectItems(os[0]->combineLatest(f, *os[1]), items);
        varxRequireItems(items, "0 1 ");
    }

    IT("works with arity 2")
    {
        const auto f = transform<String, String, String>;
        varxCollectItems(os[0]->combineLatest(f, *os[1], *os[2]), items);
        varxRequireItems(items, "0 1 2 ");
    }

    IT("works with arity 3")
    {
        const auto f = transform<String, String, String, String>;
        varxCollectItems(os[0]->combineLatest(f, *os[1], *os[2], *os[3]), items);
        varxRequireItems(items, "0 1 2 3 ");
    }

    IT("works with arity 4")
    {
        const auto f = transform<String, String, String, String, String>;
        varxCollectItems(os[0]->combineLatest(f, *os[1], *os[2], *os[3], *os[4]), items);
        varxRequireItems(items, "0 1 2 3 4 ");
    }

    IT("works with arity 5")
    {
        const auto f = transform<String, String, String, String, String, String>;
        varxCollectItems(os[0]->combineLatest(f, *os[1], *os[2], *os[3], *os[4], *os[5]), items);
        varxRequireItems(items, "0 1 2 3 4 5 ");
    }

    IT("works with arity 6")
    {
        const auto f = transform<String, String, String, String, String, String, String>;
        varxCollectItems(os[0]->combineLatest(f, *os[1], *os[2], *os[3], *os[4], *os[5], *os[6]), items);
        varxRequireItems(items, "0 1 2 3 4 5 6 ");
    }

    IT("works with arity 7")
    {
        const auto f = transform<String, String, String, String, String, String, String, String>;
        varxCollectItems(os[0]->combineLatest(f, *os[1], *os[2], *os[3], *os[4], *os[5], *os[6], *os[7]), items);
        varxRequireItems(items, "0 1 2 3 4 5 6 7 ");
    }

    IT("combines elements into a tuple when no transform is given")
    {
        auto o1 = Observable<bool>::just(true);
        auto o2 = Observable<String>::from({ "Hello", "World" });
        auto o3 = Observable<>::range(4, 6);
        auto combined = o1.combineLatest(o2, o3);
        static_assert(std::is_same<decltype(combined), Observable<std::tuple<bool, String, int>>>::value, "Combined Observable has wrong type.");

        Array<std::tuple<bool, String, int>> items;
        varxCollectItems(combined, items);
        
        CHECK(items.size() == 3);
        
        varxRequireItems(items,
                         std::make_tuple(true, "World", 4),
                         std::make_tuple(true, "World", 5),
                         std::make_tuple(true, "World", 6));
    }
}


TEST_CASE("Observable::concat",
          "[Observable][Observable::concat]")
{
    Array<var> items;

    IT("concatenates the values emitted by the source Observables")
    {
        auto observable = Observable<var>::from({ "Hello", "World" });
        auto another = Observable<var>::from({ 1.5, 2.32, 5.6 });
        varxCollectItems(observable.concat(another), items);

        varxRequireItems(items, var("Hello"), var("World"), var(1.5), var(2.32), var(5.6));
    }
}


TEST_CASE("Observable::distinctUntilChanged",
          "[Observable][Observable::distinctUntilChanged]")
{
    IT("doesn't emit consecutive duplicate integers")
    {
        Array<var> originalItems;
        Array<var> filteredItems;
        PublishSubject<var> subject;

        varxCollectItems(subject, originalItems);
        varxCollectItems(subject.distinctUntilChanged(), filteredItems);

        subject.onNext(3);
        subject.onNext(3);
        subject.onNext("3"); // Is equal to 3, due to how juce::var defines operator==
        subject.onNext(3);
        subject.onNext(3);
        subject.onNext(5);
        subject.onNext(3);

        varxRequireItems(originalItems, var(3), var(3), var("3"), var(3), var(3), var(5), var(3));
        varxRequireItems(filteredItems, var(3), var(5), var(3));
    }

    IT("doesn't emit consecutive duplicate Point<int>s")
    {
        Array<Point<int>> items;
        PublishSubject<Point<int>> subject;

        // Declare Point<int> as the type parameter, to use Point<int>::operator== for comparison:
        varxCollectItems(subject.distinctUntilChanged(), items);

        subject.onNext(Point<int>(27, 12));
        subject.onNext(Point<int>(27, 12));
        subject.onNext(Point<int>(27, 14));

        varxRequireItems(items, Point<int>(27, 12), Point<int>(27, 14));
    }
}


TEST_CASE("Observable::elementAt",
          "[Observable][Observable::elementAt]")
{
    auto observable = Observable<float>::from({ 17.4, 3.0, 1.5, 6.77 });
    Array<double> items;

    IT("emits only the item at the given index")
    {
        varxCollectItems(observable.elementAt(2), items);

        varxRequireItems(items, 1.5);
    }
}


TEST_CASE("Observable::filter",
          "[Observable][Observable::filter]")
{
    IT("filters ints")
    {
        Array<int> items;
        auto source = Observable<>::range(4, 9, 1);
        auto filtered = source.filter([](int i) {
            return (i % 2 == 0);
        });
        varxCollectItems(filtered, items);

        varxRequireItems(items, 4, 6, 8);
    }

    IT("filters Strings")
    {
        Array<String> items;
        auto source = Observable<String>::from({ "Hello", "Great", "World", "Hey" });
        auto filtered = source.filter([](String s) {
            return s[0] == 'H';
        });
        varxCollectItems(filtered, items);

        varxRequireItems(items, "Hello", "Hey");
    }

    IT("filters an Observable which emits different types")
    {
        Array<var> items;
        auto source = Observable<var>::from({ var(3), var("Hello"), var(5.43) });
        auto filtered = source.filter([](var v) {
            return v.isDouble();
        });
        varxCollectItems(filtered, items);

        varxRequireItems(items, 5.43);
    }

    IT("works with std::bind")
    {
        Array<int> items;
        auto source = Observable<>::range(14, 19);

        struct Test
        {
            bool test(int item) { return item < 17; }
        };

        Test t;
        auto predicate = std::bind(&Test::test, &t, std::placeholders::_1);
        auto filtered = source.filter(predicate);

        varxCollectItems(filtered, items);
        varxRequireItems(items, 14, 15, 16);
    }
}


TEST_CASE("Observable::flatMap",
          "[Observable][Observable::flatMap]")
{
    Array<String> items;

    IT("merges the values emitted by the returned Observables")
    {
        auto o = Observable<String>::from({ "Hello", "World" }).flatMap([](String s) {
            return Observable<String>::from({ s.toLowerCase(), s.toUpperCase() + "!" });
        });
        varxCollectItems(o, items);

        varxRequireItems(items, "hello", "HELLO!", "world", "WORLD!");
    }
}


TEST_CASE("Observable::map",
          "[Observable][Observable::map]")
{
    Array<int> items;
    auto source = Observable<>::range(4, 7, 2);

    IT("emits values synchronously")
    {
        auto mapped = source.map([](int i) { return i * 1.5; });
        varxCollectItems(mapped, items);

        varxRequireItems(items, 6.0, 9.0, 10.5);
    }
}


TEST_CASE("Interaction between Observable::map and Observable::switchOnNext",
          "[Observable][Observable::map][Observable::switchOnNext]")
{
    IT("supports returning Observables in map, even nested twice")
    {
        Array<String> items;
        auto source = Observable<int>::just(1);
        auto nested = source.map([](int i) {
            return Observable<String>::just("Hello").map([i](String s) {
                return Observable<var>::just(String(i) + " " + s);
            });
        });

        // Unwrap twice
        auto unwrapped = nested.switchOnNext().switchOnNext();
        varxCollectItems(unwrapped, items);

        varxRequireItems(items, "1 Hello");
    }

    IT("continues to emit items after the source Observable is gone")
    {
        Array<int> items;
        auto source = std::make_shared<Observable<int>>(Observable<int>::just(17));
        auto mapped = source->map([](int next) {
            return Observable<int>::create([next](Observer<int> observer) {
                MessageManager::getInstance()->callAsync([observer, next]() {
                    observer.onNext(next * 3);
                });
            });
        });
        auto unwrapped = mapped.switchOnNext();
        varxCollectItems(unwrapped, items);

        // There should be no items before running dispatch loop
        CHECK(items.isEmpty());

        source.reset();
        varxRunDispatchLoop();

        // The item should be emitted, although there's no reference to the source anymore
        varxRequireItems(items, 17 * 3);
    }
}


TEST_CASE("Observable::reduce",
          "[Observable][Observable::reduce]")
{
    Array<int> items;

    IT("reduces emitted items")
    {
        auto observable = Observable<int>::from({ 10, 100, 1000 }).reduce(2, [](int accum, int next) {
            return accum + next;
        });

        varxCollectItems(observable, items);

        varxRequireItems(items, 1112);
    }
}


TEST_CASE("Observable::scan",
          "[Observable][Observable::scan]")
{
    Array<int> items;

    IT("applies the transform function to the inputs")
    {
        auto o = Observable<>::range(1, 5).scan(10, [](int accum, int currentValue) {
            return accum + currentValue;
        });
        varxCollectItems(o, items);

        varxRequireItems(items, 11, 13, 16, 20, 25);
    }
}


TEST_CASE("Observable::skip",
          "[Observable][Observable::skip]")
{
    Array<int> items;

    IT("skips the first 4 items")
    {
        auto o = Observable<int>::from({ 4, 7, 2, 1, 19, 1, 33, 4 }).skip(4);
        varxCollectItems(o, items);

        varxRequireItems(items, 19, 1, 33, 4);
    }
}


TEST_CASE("Observable::skipUntil",
          "[Observable][Observable::skipUntil]")
{
    Array<String> items;

    IT("skips until another Observable emits an item")
    {
        PublishSubject<String> subject;
        PublishSubject<var> trigger;

        varxCollectItems(subject.skipUntil(trigger), items);

        // Emit some items, these should NOT be received
        subject.onNext("Not");
        subject.onNext("Getting");
        subject.onNext("This");

        // Trigger
        trigger.onNext(var::undefined());

        // Emit more items, these should be received
        subject.onNext("These");
        subject.onNext("Are");
        subject.onNext("Received");

        varxRequireItems(items, "These", "Are", "Received");
    }
}


TEST_CASE("Observable::startWith",
          "[Observable][Observable::startWith]")
{
    Array<int> items;
    auto observable = Observable<int>::from({ 17, 3 });

    IT("prepends items to an existing Observable")
    {
        varxCollectItems(observable.startWith(6, 4, 7, 2), items);

        varxRequireItems(items, 6, 4, 7, 2, 17, 3);
    }
}


TEST_CASE("Observable::takeLast",
          "[Observable][Observable::takeLast]")
{
    Array<String> items;
    auto observable = Observable<String>::from({ "First", "Another", "And one more", "Last item" });

    IT("takes the last 2 emitted items")
    {
        varxCollectItems(observable.takeLast(2), items);

        varxRequireItems(items, "And one more", "Last item");
    }
}


TEST_CASE("Observable::takeUntil",
          "[Observable][Observable::takeUntil]")
{
    Array<String> items;

    IT("emits until another Observable emits an item")
    {
        PublishSubject<String> subject;
        PublishSubject<String> trigger;

        varxCollectItems(subject.takeUntil(trigger), items);

        // Emit some items, these should be received
        subject.onNext("These");
        subject.onNext("Are");
        subject.onNext("Received");

        // Trigger
        trigger.onNext("Hey stop!");

        // Emit more items, these should NOT be received
        subject.onNext("Not");
        subject.onNext("Getting");
        subject.onNext("This");

        varxRequireItems(items, "These", "Are", "Received");
    }
}


TEST_CASE("Observable::takeWhile",
          "[Observable][Observable::takeWhile]")
{
    Array<int> items;

    IT("emits items as long as the predicate returns true")
    {
        PublishSubject<int> subject;
        const auto predicate = [](int i) {
            return i <= 10;
        };

        varxCollectItems(subject.takeWhile(predicate), items);

        // These should be emitted
        subject.onNext(4);
        subject.onNext(7);
        subject.onNext(10);

        // These shouldn't be emitted, because predicate(11) == false
        subject.onNext(11);
        subject.onNext(3);
        subject.onNext(7);

        varxRequireItems(items, 4, 7, 10);
    }
}


TEST_CASE("Observable::withLatestFrom",
          "[Observable][Observable::withLatestFrom]")
{
    Array<String> items;
    PublishSubject<String> s1;
    PublishSubject<String> s2;

    IT("only emits when the first Observable emits")
    {
        const auto f = transform<String, String>;
        varxCollectItems(s1.withLatestFrom(s2, f), items);
        CHECK(items.isEmpty());
        s2.onNext("World!");
        CHECK(items.isEmpty());
        s1.onNext("Hello ");

        varxRequireItems(items, "Hello World!");
    }
}


TEST_CASE("Observable::zip",
          "[Observable][Observable::zip]")
{
    Array<String> items;

    IT("zips three Observables together")
    {
        PublishSubject<String> strings;
        PublishSubject<int> ints;
        PublishSubject<double> doubles;
        const auto combine = [](String s, int i, double d) {
            return "s=" + s + "; i=" + String(i) + "; d=" + String(d);
        };

        varxCollectItems(strings.zip(ints, doubles, combine), items);

        // First item should be emitted when all three Observables have emitted once
        strings.onNext("a");
        CHECK(items.isEmpty());
        ints.onNext(1);
        CHECK(items.isEmpty());
        doubles.onNext(0.1);
        varxCheckItems(items, "s=a; i=1; d=0.1");

        // Second item should be emitted when all three Observables have emitted twice
        doubles.onNext(0.25);
        CHECK(items.size() == 1);
        ints.onNext(57);
        CHECK(items.size() == 1);
        strings.onNext("x");
        varxRequireItems(items, "s=a; i=1; d=0.1", "s=x; i=57; d=0.25");
    }
}
