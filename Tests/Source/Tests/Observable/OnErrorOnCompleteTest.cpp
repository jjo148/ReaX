#include "../../Other/TestPrefix.h"


TEST_CASE("Observable onError",
          "[Observable][onError]")
{
    // Create an Observable that throws on subscribe
    auto syncThrow = Observable<>::create<String>([](Observer<String>) { throw std::runtime_error("Error!"); });

    IT("calls onError on subscribe")
    {
        REQUIRE_THROWS_WITH(syncThrow.subscribe([](String) {}, std::rethrow_exception), "Error!");
    }

    IT("takes an onError handler and calls it without throwing")
    {
        bool called = false;
        syncThrow.subscribe([](String) {}, [&](std::exception_ptr) { called = true; });

        REQUIRE(called);
    }

    IT("calls onError asynchronously")
    {
        // Create an Observable that throws asynchronously
        auto asyncThrow = Observable<>::create<int>([](Observer<int> observer) {
            MessageManager::getInstance()->callAsync([observer]() {
                observer.onNext(3);
            });
        });
        asyncThrow = asyncThrow.map([](int i) -> int {
            throw std::runtime_error("Async Error!");
        });

        bool called = false;
        asyncThrow.subscribe([](int) {}, [&](std::exception_ptr) { called = true; });

        CHECK_FALSE(called);
        varxRunDispatchLoopUntil(called);
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
        Observable<>::just(2).subscribe([](int) {}, [](std::exception_ptr) {}, onComplete);
        REQUIRE(called);
    }
}
