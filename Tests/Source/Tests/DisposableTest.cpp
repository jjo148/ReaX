#include "../Other/TestPrefix.h"


TEST_CASE("Subscription",
          "[Subscription]")
{
    // Create Observable which emits a single value asynchronously
    auto observable = std::make_shared<Observable<String>>(Observable<String>::create([](Observer<String> observer) {
        MessageManager::getInstance()->callAsync([observer]() {
            observer.onNext("Value");
        });
    }));

    // Subscribe to it
    Array<String> values;
    auto subscription = std::make_shared<Subscription>(observable->subscribe([&](String value) {
        values.add(value);
    }));

    IT("received values while being subscribed")
    {
        Reaction_RunDispatchLoopUntil(!values.isEmpty());

        Reaction_RequireValues(values, "Value");
    }

    IT("does not receive values after disposing")
    {
        subscription->unsubscribe();
        Reaction_RunDispatchLoop(20);

        REQUIRE(values.isEmpty());
    }

    IT("takes ownership when move constructing")
    {
        Subscription other = std::move(*subscription);
        other.unsubscribe();
        Reaction_RunDispatchLoop(20);

        REQUIRE(values.isEmpty());
    }

    IT("does not unsubscribe when being destroyed")
    {
        subscription.reset();
        Reaction_RunDispatchLoopUntil(!values.isEmpty());

        Reaction_RequireValues(values, "Value");
    }

    IT("continues to receive values after the Observable is gone")
    {
        observable.reset();
        Reaction_RunDispatchLoopUntil(!values.isEmpty());

        Reaction_RequireValues(values, "Value");
    }

    // Unsubscribe after each IT(), to prevent old subscriptions from filling the values array
    if (subscription)
        subscription->unsubscribe();
}


TEST_CASE("DisposeBag",
          "[DisposeBag]")
{
    auto disposeBag = std::make_shared<DisposeBag>();

    // Create Observable which emits a single value asynchronously
    auto observable = Observable<String>::create([](Observer<String> observer) {
        MessageManager::getInstance()->callAsync([observer]() {
            observer.onNext("Value");
        });
    });

    // Subscribe to it
    Array<String> values;
    observable.subscribe([&](String value) {
                  values.add(value);
              })
        .disposedBy(*disposeBag);

    IT("received values while not destroyed")
    {
        Reaction_RunDispatchLoopUntil(!values.isEmpty());

        Reaction_RequireValues(values, "Value");
    }

    IT("does not receive values after being destroyed")
    {
        disposeBag.reset();
        Reaction_RunDispatchLoop(20);

        REQUIRE(values.isEmpty());
    }

    IT("can dispose multiple Subscriptions")
    {
        for (int i = 0; i < 5; ++i) {
            observable.subscribe([&](String value) {
                          values.add(value);
                      })
                .disposedBy(*disposeBag);
        }

        disposeBag.reset();
        Reaction_RunDispatchLoop(20);

        REQUIRE(values.isEmpty());
    }
}
