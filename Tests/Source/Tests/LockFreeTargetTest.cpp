#include "../Other/TestPrefix.h"

TEST_CASE("LockFreeTarget",
          "[LockFreeTarget][ReleasePool]")
{
    IT("retrieves values for primitive types")
    {
        LockFreeTarget<float> target;
        BehaviorSubject<float> subject(5.467f);
        subject.subscribe(target);
        CHECK(target.getValue() == 5.467f);
        
        subject.onNext(100.24f);
        CHECK(target.getValue() == 100.24f);
        
        subject.onNext(-14.274f);
        REQUIRE(target.getValue() == -14.274f);
    }
    
    IT("retrieves String values")
    {
        LockFreeTarget<String> target;
        PublishSubject<String> subject;
        subject.subscribe(target);
        
        subject.onNext("Hello");
        CHECK(target.getValue() == "Hello");
        
        subject.onNext("World!");
        REQUIRE(target.getValue() == "World!");
    }
    
    IT("retrieves values for other non-primitive types")
    {
        LockFreeTarget<Point<int>> target;
        PublishSubject<Point<int>> subject;
        subject.subscribe(target);
        
        subject.onNext(Point<int>(43, 29));
        CHECK(target.getValue() == Point<int>(43, 29));
        
        subject.onNext(Point<int>(18, -5));
        REQUIRE(target.getValue() == Point<int>(18, -5));
    }
    
    
    IT("doesn't use the ReleasePool for primitive types")
    {
        // Make sure ReleasePool is initially empty
        detail::ReleasePool::get().cleanup();
        CHECK(detail::ReleasePool::get().size() == 0);
        
        LockFreeTarget<int> intTarget;
        LockFreeTarget<bool> boolTarget;
        LockFreeTarget<double> doubleTarget;
        
        PublishSubject<var> subject;
        subject.subscribe(intTarget);
        subject.subscribe(boolTarget);
        subject.subscribe(doubleTarget);
        
        subject.onNext(3.474);
        
        REQUIRE(detail::ReleasePool::get().size() == 0);
    }
    
    IT("uses the ReleasePool for non-primitive types")
    {
        // Make sure ReleasePool is initially empty
        detail::ReleasePool::get().cleanup();
        CHECK(detail::ReleasePool::get().size() == 0);
        
        // After one onNext, there should be 1 item in the ReleasePool
        PublishSubject<Point<float>> pointSubject;
        LockFreeTarget<Point<float>> pointTarget;
        pointSubject.subscribe(pointTarget);
        pointSubject.onNext(Point<float>(4.52f, 1.23f));
        REQUIRE(detail::ReleasePool::get().size() == 1);
        
        // After another onNext, there should be 2 items
        PublishSubject<String> stringSubject;
        LockFreeTarget<String> stringTarget;
        stringSubject.subscribe(stringTarget);
        stringSubject.onNext("Hello");
        REQUIRE(detail::ReleasePool::get().size() == 2);
        
        // After another onNext, there should be 3 items
        stringSubject.onNext("World!");
        REQUIRE(detail::ReleasePool::get().size() == 3);
        
        // After a cleanup, there should be just 2 items again
        detail::ReleasePool::get().cleanup();
        REQUIRE(detail::ReleasePool::get().size() == 2);
    }
}
