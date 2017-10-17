#include "../../Other/TestPrefix.h"


TEST_CASE("Observable onError",
          "[Observable][onError]")
{
    // Create an Observable that throws on subscribe
    auto syncThrow = TypedObservable<String>::create([](TypedObserver<String>) { throw std::runtime_error("Error!"); });

    IT("calls onError on subscribe")
    {
        REQUIRE_THROWS_WITH(syncThrow.subscribe([](String) {}, std::rethrow_exception), "Error!");
    }

    IT("takes an onError handler and calls it without throwing")
    {
        bool called = false;
        syncThrow.subscribe([](String) {}, [&](Error) { called = true; });

        REQUIRE(called);
    }

    IT("calls onError asynchronously")
    {
        // Create an Observable that throws asynchronously
        auto asyncThrow = TypedObservable<int>::create([](TypedObserver<int> observer) {
            MessageManager::getInstance()->callAsync([observer]() {
                observer.onNext(3);
            });
        });
        asyncThrow = asyncThrow.map([](int i) {
            throw std::runtime_error("Async Error!");
            return i;
        });

        bool called = false;
        asyncThrow.subscribe([](int) {}, [&](Error) { called = true; });

        CHECK_FALSE(called);
        varxRunDispatchLoop();
        REQUIRE(called);
    }
}


TEST_CASE("Observable onComplete",
          "[Observable][onComplete]")
{
    bool called = false;
    auto onComplete = [&]() {
        called = true;
    };

    IT("calls onComplete synchronously")
    {
        TypedObservable<int>::just(2).subscribe([](int) {}, [](Error) {}, onComplete);
        REQUIRE(called);
    }
}
