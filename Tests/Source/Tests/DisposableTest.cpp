#include "../Other/TestPrefix.h"


TEST_CASE("Subscription",
          "[Subscription]")
{
    // Create Observable which emits a single item asynchronously
    auto observable = std::make_shared<Observable<String>>(Observable<>::create<String>([](Observer<String> observer) {
        MessageManager::getInstance()->callAsync([observer]() {
            observer.onNext("Item");
        });
    }));

    // Subscribe to it
    Array<String> items;
    auto subscription = std::make_shared<Subscription>(observable->subscribe([&](String item) {
        items.add(item);
    }));

    IT("received items while being subscribed")
    {
        varxRunDispatchLoopUntil(!items.isEmpty());

        varxRequireItems(items, "Item");
    }

    IT("does not receive items after disposing")
    {
        subscription->unsubscribe();
        varxRunDispatchLoop(20);

        REQUIRE(items.isEmpty());
    }

    IT("takes ownership when move constructing")
    {
        Subscription other = std::move(*subscription);
        other.unsubscribe();
        varxRunDispatchLoop(20);

        REQUIRE(items.isEmpty());
    }

    IT("does not unsubscribe when being destroyed")
    {
        subscription.reset();
        varxRunDispatchLoopUntil(!items.isEmpty());

        varxRequireItems(items, "Item");
    }

    IT("continues to receive items after the Observable is gone")
    {
        observable.reset();
        varxRunDispatchLoopUntil(!items.isEmpty());

        varxRequireItems(items, "Item");
    }

    // Unsubscribe after each IT(), to prevent old subscriptions from filling the items array
    if (subscription)
        subscription->unsubscribe();
}


TEST_CASE("DisposeBag",
          "[DisposeBag]")
{
    auto disposeBag = std::make_shared<DisposeBag>();

    // Create Observable which emits a single item asynchronously
    auto observable = Observable<>::create<String>([](Observer<String> observer) {
        MessageManager::getInstance()->callAsync([observer]() {
            observer.onNext("Item");
        });
    });

    // Subscribe to it
    Array<String> items;
    observable.subscribe([&](String item) {
                  items.add(item);
              })
        .disposedBy(*disposeBag);

    IT("received items while not destroyed")
    {
        varxRunDispatchLoopUntil(!items.isEmpty());

        varxRequireItems(items, "Item");
    }

    IT("does not receive items after being destroyed")
    {
        disposeBag.reset();
        varxRunDispatchLoop(20);

        REQUIRE(items.isEmpty());
    }

    IT("can dispose multiple Subscriptions")
    {
        for (int i = 0; i < 5; ++i) {
            observable.subscribe([&](String item) {
                          items.add(item);
                      })
                .disposedBy(*disposeBag);
        }

        disposeBag.reset();
        varxRunDispatchLoop(20);

        REQUIRE(items.isEmpty());
    }
}
