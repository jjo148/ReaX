#include "../Other/TestPrefix.h"


TEST_CASE("Subscription",
          "[Subscription]")
{
    // Create Observable which emits a single value asynchronously
    auto observable = std::make_shared<Observable<String>>(Observable<>::create<String>([](Observer<String> observer) {
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
        varxRunDispatchLoopUntil(!values.isEmpty());

        varxRequireValues(values, "Value");
    }

    IT("does not receive values after disposing")
    {
        subscription->unsubscribe();
        varxRunDispatchLoop(20);

        REQUIRE(values.isEmpty());
    }

    IT("takes ownership when move constructing")
    {
        Subscription other = std::move(*subscription);
        other.unsubscribe();
        varxRunDispatchLoop(20);

        REQUIRE(values.isEmpty());
    }

    IT("does not unsubscribe when being destroyed")
    {
        subscription.reset();
        varxRunDispatchLoopUntil(!values.isEmpty());

        varxRequireValues(values, "Value");
    }

    IT("continues to receive values after the Observable is gone")
    {
        observable.reset();
        varxRunDispatchLoopUntil(!values.isEmpty());

        varxRequireValues(values, "Value");
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
    auto observable = Observable<>::create<String>([](Observer<String> observer) {
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
        varxRunDispatchLoopUntil(!values.isEmpty());

        varxRequireValues(values, "Value");
    }

    IT("does not receive values after being destroyed")
    {
        disposeBag.reset();
        varxRunDispatchLoop(20);

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
        varxRunDispatchLoop(20);

        REQUIRE(values.isEmpty());
    }
}
