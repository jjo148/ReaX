#include "../Other/TestPrefix.h"

TEST_CASE("LockFreeSource",
          "[LockFreeSource]")
{
    CONTEXT("Primitive type (int)")
    {
        Array<int> items;
        LockFreeSource<int> source(3);
        varxCollectItems(source, items);
        CHECK(items.isEmpty());
        
        IT("emits items asynchronously via the Observable")
        {
            auto congestionPolicy = CongestionPolicy::Allocate;
            for (auto i : {4, 58, 18, -3})
                source.onNext(i, congestionPolicy);
            
            CHECK(items.isEmpty());
            
            varxRunDispatchLoopUntil(items.size() == 4);
            varxRequireItems(items, 4, 58, 18, -3);
        }
        
        IT("can discard the oldest items")
        {
            auto congestionPolicy = CongestionPolicy::DropOldest;
            for (int i = 0; i < 100; ++i)
                source.onNext(i * 17, congestionPolicy);
            
            // The ConcurrentQueue seems to round the capacity to 4
            varxRunDispatchLoopUntil(items.size() == 4);
            varxRequireItems(items, 96 * 17, 97 * 17, 98 * 17, 99 * 17);
        }
        
        IT("can discard the newest items")
        {
            auto congestionPolicy = CongestionPolicy::DropNewest;
            // Fill queue
            for (int i = 0; i < 100; ++i)
                source.onNext(i, congestionPolicy);
            
            // Add another item
            source.onNext(382, congestionPolicy);
            
            varxRunDispatchLoop(1);
            
            // The newest item should be discarded
            REQUIRE(items.getLast() != 382);
        }
    }
    
    CONTEXT("move semantics")
    {
        // Create source
        CopyAndMoveConstructible::Counters unused;
        LockFreeSource<CopyAndMoveConstructible> source(10, CopyAndMoveConstructible(&unused));
        
        // Create counting object
        CopyAndMoveConstructible::Counters counters;
        CopyAndMoveConstructible item(&counters);
        
        IT("uses the copy overload for an lvalue")
        {
            source.onNext(item, CongestionPolicy::Allocate);
            
            REQUIRE(counters.numCopyConstructions == 1);
            REQUIRE(counters.numCopyAssignments == 0);
            REQUIRE(counters.numMoveConstructions == 0);
            REQUIRE(counters.numMoveAssignments == 0);
        }
        
        IT("uses the move overload for an rvalue")
        {
            source.onNext(std::move(item), CongestionPolicy::Allocate);
            
            REQUIRE(counters.numCopyConstructions == 0);
            REQUIRE(counters.numCopyAssignments == 0);
            REQUIRE(counters.numMoveConstructions == 1);
            REQUIRE(counters.numMoveAssignments == 0);
        }
    }
}
