/*
  ==============================================================================

    rxjuce_LifetimeWatcherPool.h
    Created: 27 Apr 2017 7:14:37am
    Author:  Martin Finke

  ==============================================================================
*/

#pragma once

#include "rxjuce_LifetimeWatcher.h"

RXJUCE_NAMESPACE_BEGIN

/**
 Keeps a list of LifetimeWatcher instances and periodically checks whether they have expired.
 
 An expired watcher is removed from the list and deleted. You can send a notification by implementing the destructor of a LifetimeWatcher subclass.
 
 @see LifetimeWatcher, ReferenceCountedObjectLifetimeWatcher, WeakReferenceLifetimeWatcher
 */
class LifetimeWatcherPool : private juce::Timer
{
public:
	static LifetimeWatcherPool& getInstance();

	void add(const LifetimeWatcher *watcher);

private:
	LifetimeWatcherPool();
	
	void timerCallback() override;
	
	juce::OwnedArray<const LifetimeWatcher> watchers;
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LifetimeWatcherPool)
};

RXJUCE_NAMESPACE_END
