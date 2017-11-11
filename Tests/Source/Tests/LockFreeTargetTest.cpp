#include "../Other/TestPrefix.h"

TEST_CASE("LockFreeTarget",
          "[LockFreeTarget][ReleasePool]")
{
    IT("retrieves primitive values")
    {
        BehaviorSubject<float> subject(5.467f);
        LockFreeTarget<float> target;
        subject.subscribe(target);
        float value;
        CHECK(target.tryDequeueAll(value));
        CHECK(value == 5.467f);
        
        subject.onNext(100.24f);
        CHECK(target.tryDequeue(value));
        CHECK(value == 100.24f);
        
        subject.onNext(-14.274f);
        CHECK(target.tryDequeueAll(value));
        REQUIRE(value == -14.274f);
    }
    
    IT("retrieves String values")
    {
        PublishSubject<String> subject;
        LockFreeTarget<String> target;
        subject.subscribe(target);
        
        subject.onNext("Hello");
        String value;
        CHECK(target.tryDequeue(value));
        CHECK(value == "Hello");
        
        // Enqueue 3 values
        subject.onNext("This should be discarded.");
        subject.onNext("This should be discarded, too.");
        subject.onNext("World!");
        
        // Dequeue all of them, should get the last value
        CHECK(target.tryDequeueAll(value));
        REQUIRE(value == "World!");
        
        // Should be empty now, and not touch "value" parameter
        String anotherValue("anotherValue");
        CHECK_FALSE(target.tryDequeue(anotherValue));
        CHECK_FALSE(target.tryDequeueAll(anotherValue));
        REQUIRE(value == "World!");
        REQUIRE(anotherValue == "anotherValue");
    }
    
    IT("retrieves other non-primitive values")
    {
        PublishSubject<Point<int>> subject;
        LockFreeTarget<Point<int>> target;
        subject.subscribe(target);
        
        subject.onNext(Point<int>(43, 29));
        Point<int> value;
        CHECK(target.tryDequeueAll(value));
        CHECK(value == Point<int>(43, 29));
        
        // Enqueue 3 values
        subject.onNext(Point<int>(18, -5));
        subject.onNext(Point<int>(-163, 122));
        subject.onNext(Point<int>(0, 774));
        
        // Dequeue 3 values
        CHECK(target.tryDequeue(value));
        CHECK(value == Point<int>(18, -5));
        CHECK(target.tryDequeue(value));
        REQUIRE(value == Point<int>(-163, 122));
        CHECK(target.tryDequeue(value));
        REQUIRE(value == Point<int>(0, 774));
        
        // Should be empty now
        Point<int> anotherValue(371, 4819);
        CHECK_FALSE(target.tryDequeue(anotherValue));
        CHECK_FALSE(target.tryDequeueAll(anotherValue));
        REQUIRE(value == Point<int>(0, 774));
        REQUIRE(anotherValue == Point<int>(371, 4819));
    }
    
    IT("can convert between convertible types")
    {
        LockFreeTarget<int> target;
        int64 value;
        
        target.onNext(312);
        CHECK(target.tryDequeueAll(value));
        REQUIRE(value == 312);
    }
    
    CONTEXT("queue is empty")
    {
        LockFreeTarget<int64> target;
        int64 value;
        
        IT("returns false when calling tryDequeue")
        {
            // Call multiple times, just to check
            CHECK_FALSE(target.tryDequeue(value));
            REQUIRE_FALSE(target.tryDequeue(value));
        }
        
        IT("returns false when calling tryDequeueAll")
        {
            // Call multiple times, just to check
            CHECK_FALSE(target.tryDequeueAll(value));
            REQUIRE_FALSE(target.tryDequeueAll(value));
        }
        
        CONTEXT("after retrieving and dequeing values")
        {
            IT("returns false after retrieving and dequeueing 1 value")
            {
                target.onNext(45009);
                CHECK(target.tryDequeue(value));
                CHECK_FALSE(target.tryDequeue(value));
                CHECK_FALSE(target.tryDequeueAll(value));
            }
            
            IT("returns false after retrieving and dequeueing 3 value")
            {
                target.onNext(231);
                target.onNext(12310);
                target.onNext(-9481);
                
                CHECK(target.tryDequeueAll(value));
                CHECK_FALSE(target.tryDequeue(value));
                CHECK_FALSE(target.tryDequeueAll(value));
            }
        }
    }
    
    CONTEXT("move semantics")
    {
        LockFreeTarget<CopyAndMoveConstructible> target;
        CopyAndMoveConstructible::Counters counters;
        
        IT("copies when passing an rvalue to onNext")
        {
            target.onNext(CopyAndMoveConstructible(&counters));
            
            CHECK(counters.numCopyConstructions == 1);
            CHECK(counters.numMoveConstructions == 1);
            CHECK(counters.numCopyAssignments == 0);
            REQUIRE(counters.numMoveAssignments == 0);
            
            IT("uses move assignment when emptying the queue, and makes no further copies")
            {
                CopyAndMoveConstructible value(nullptr);
                target.tryDequeueAll(value);
                
                CHECK(counters.numCopyConstructions == 1);
                CHECK(counters.numMoveConstructions == 1);
                CHECK(counters.numCopyAssignments == 0);
                REQUIRE(counters.numMoveAssignments == 1);
            }
        }
    }
}
