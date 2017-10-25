/*
  ==============================================================================

    TestPrefix.h
    Created: 27 Apr 2017 7:55:11am
    Author:  Martin Finke

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "catch.hpp"

using namespace juce;
using namespace varx;

#define CONTEXT(desc) SECTION(std::string(" Context: ") + desc, "")
#define IT(desc) SECTION(std::string("       It ") + desc, "")

/** Subscribes to an Observable and collects all emitted items into a given Array. */
#define varxCollectItems(__observable, __arrayName) \
    DisposeBag JUCE_JOIN_MACRO(__arrayName, JUCE_JOIN_MACRO(Disposable_, __LINE__)); \
    (__observable).subscribe([&__arrayName](const decltype(__arrayName.getFirst())& item) { __arrayName.add(item); }).disposedBy(JUCE_JOIN_MACRO(__arrayName, JUCE_JOIN_MACRO(Disposable_, __LINE__)));

/** REQUIREs that a given Array is equal to the list of passed items. */
#define varxRequireItems(__arrayName, ...) REQUIRE(__arrayName == decltype(__arrayName)({ __VA_ARGS__ }))

/** CHECKs that a given Array is equal to the list of passed items. */
#define varxCheckItems(__arrayName, ...) CHECK(__arrayName == decltype(__arrayName)({ __VA_ARGS__ }))


/** Runs the JUCE dispatch loop for a given time, to process async callbacks. */
inline void varxRunDispatchLoop(int millisecondsToRunFor = 0)
{
    MessageManager::getInstance()->runDispatchLoopUntil(millisecondsToRunFor);
}

/** Runs the JUCE dispatch loop until a given condition is fulfilled. */
#define varxRunDispatchLoopUntil(__condition) \
    { \
        const auto startTime = juce::Time::getMillisecondCounter(); \
        while (!(__condition) && Time::getMillisecondCounter() < startTime + 5 * 1000) { \
            varxRunDispatchLoop(5); \
        } \
    } \
    REQUIRE(__condition);

/** The app window for running the tests. */
class TestWindow : public DocumentWindow, private DeletedAtShutdown
{
public:
    static TestWindow& getInstance()
    {
        static TestWindow* window = new TestWindow();
        return *window;
    }

    void addAndMakeVisible(Component& component)
    {
        getContentComponent()->addAndMakeVisible(component);
    }

private:
    TestWindow()
    : DocumentWindow("varx-Tests", Colours::white, DocumentWindow::TitleBarButtons::closeButton, true)
    {
        ScopedPointer<Component> component(new Component());
        component->setSize(1, 1);
        setUsingNativeTitleBar(true);
        setContentOwned(component.release(), true);
        setVisible(true);
    }
};

// Dummy struct that just counts copy and move constructions
struct CopyAndMoveConstructible
{
    struct Counters
    {
        int numCopyConstructions = 0;
        int numMoveConstructions = 0;
        int numCopyAssignments = 0;
        int numMoveAssignments = 0;
    };

    CopyAndMoveConstructible(Counters* counters)
    : counters(counters)
    {}

    // Copy Constructor
    CopyAndMoveConstructible(const CopyAndMoveConstructible& other)
    : counters(other.counters)
    {
        counters->numCopyConstructions++;
    }

    // Move constructor
    CopyAndMoveConstructible(CopyAndMoveConstructible&& other)
    : counters(std::move(other.counters))
    {
        counters->numMoveConstructions++;
    }

    // Copy assignment
    CopyAndMoveConstructible& operator=(const CopyAndMoveConstructible& other)
    {
        counters = other.counters;
        counters->numCopyAssignments++;
        
        return *this;
    }

    // Move assignment
    CopyAndMoveConstructible& operator=(CopyAndMoveConstructible&& other)
    {
        counters = std::move(other.counters);
        counters->numMoveAssignments++;
        
        return *this;
    }

    Counters* counters;
};
