#include "../../Other/TestPrefix.h"


TEST_CASE("Observable::observeOn",
          "[Observable][Observable::observeOn]")
{
    auto observable = Observable<int>::from({ 1, 2, 3 });
    Array<int> values;

    IT("can schedule to a background thread and two new threads")
    {
        CHECK(MessageManager::getInstance()->isThisTheMessageThread());
        auto messageThreadID = Thread::getCurrentThreadId();
        CHECK(messageThreadID != nullptr);

        Thread::ThreadID backgroundThreadID = nullptr;
        Thread::ThreadID newThreadID = nullptr;
        Thread::ThreadID anotherNewThreadID = nullptr;

        // Observe on background thread
        auto onBackgroundThread = observable.observeOn(Scheduler::backgroundThread()).map([&](int i) {
            backgroundThreadID = Thread::getCurrentThreadId();
            return i * 2;
        });

        // Observe on new thread
        auto onNewThread = onBackgroundThread.observeOn(Scheduler::newThread()).map([&](int i) {
            newThreadID = Thread::getCurrentThreadId();
            return i * 3;
        });

        auto onAnotherNewThread = onNewThread.observeOn(Scheduler::newThread()).map([&](int i) {
            anotherNewThreadID = Thread::getCurrentThreadId();
            return i * 4;
        });

        // Wait blocking
        values = onAnotherNewThread.toArray();

        // Check that no thread IDs are equal, and all are != nullptr
        SortedSet<Thread::ThreadID> threadIDs;
        threadIDs.add(messageThreadID);
        threadIDs.add(backgroundThreadID);
        threadIDs.add(newThreadID);
        threadIDs.add(anotherNewThreadID);
        threadIDs.add(nullptr);

        REQUIRE(threadIDs.size() == 5);
        Reaction_RequireValues(values, 24, 48, 72);
    }

    IT("can schedule to the message thread")
    {
        auto onMessageThread = observable.observeOn(Scheduler::messageThread()).map([](int i) {
            return i * 2;
        });
        Reaction_CollectValues(onMessageThread, values);

        // There shouldn't be any values yet, because observeOn is asynchronous
        CHECK(values.isEmpty());

        // Wait for values to be emitted asynchronously
        Reaction_RunDispatchLoopUntil(values.size() == 3);

        Reaction_RequireValues(values, 2, 4, 6);
    }
}
