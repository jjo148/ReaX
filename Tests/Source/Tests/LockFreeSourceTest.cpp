#include "../Other/TestPrefix.h"

TEST_CASE("LockFreeSource",
          "[LockFreeSource]")
{
    CONTEXT("Primitive type (int)")
    {
        Array<int> values;
        LockFreeSource<int> source(3);
        varxCollectValues(source, values);
        CHECK(values.isEmpty());
        
        IT("emits values asynchronously via the Observable")
        {
            auto congestionPolicy = CongestionPolicy::Allocate;
            for (auto i : {4, 58, 18, -3})
                source.onNext(i, congestionPolicy);
            
            CHECK(values.isEmpty());
            
            varxRunDispatchLoopUntil(values.size() == 4);
            varxRequireValues(values, 4, 58, 18, -3);
        }
        
        IT("can discard the oldest values")
        {
            auto congestionPolicy = CongestionPolicy::DropOldest;
            for (int i = 0; i < 100; ++i)
                source.onNext(i * 17, congestionPolicy);
            
            // The ConcurrentQueue seems to round the capacity to 4
            varxRunDispatchLoopUntil(values.size() == 4);
            varxRequireValues(values, 96 * 17, 97 * 17, 98 * 17, 99 * 17);
        }
        
        IT("can discard the newest values")
        {
            auto congestionPolicy = CongestionPolicy::DropNewest;
            // Fill queue
            for (int i = 0; i < 100; ++i)
                source.onNext(i, congestionPolicy);
            
            // Add another value
            source.onNext(382, congestionPolicy);
            
            varxRunDispatchLoop(1);
            
            // The newest value should be discarded
            REQUIRE(values.getLast() != 382);
        }
    }
    
    CONTEXT("move semantics")
    {
        // Create source
        CopyAndMoveConstructible::Counters unused;
        LockFreeSource<CopyAndMoveConstructible> source(10, CopyAndMoveConstructible(&unused));
        
        // Create counting object
        CopyAndMoveConstructible::Counters counters;
        CopyAndMoveConstructible value(&counters);
        
        IT("uses the copy overload for an lvalue")
        {
            source.onNext(value, CongestionPolicy::Allocate);
            
            REQUIRE(counters.numCopyConstructions == 1);
            REQUIRE(counters.numCopyAssignments == 0);
            REQUIRE(counters.numMoveConstructions == 0);
            REQUIRE(counters.numMoveAssignments == 0);
        }
        
        IT("uses the move overload for an rvalue")
        {
            source.onNext(std::move(value), CongestionPolicy::Allocate);
            
            REQUIRE(counters.numCopyConstructions == 0);
            REQUIRE(counters.numCopyAssignments == 0);
            REQUIRE(counters.numMoveConstructions == 1);
            REQUIRE(counters.numMoveAssignments == 0);
        }
    }
}
