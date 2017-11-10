#if 0
/*
BEGIN_JUCE_MODULE_DECLARATION
ID: reaction
vendor: martin-finke
version: 0.7.0
name: Reaction
description: Reactive Extensions (Rx) for JUCE.
dependencies: juce_core, juce_data_structures, juce_events, juce_graphics, juce_gui_basics
website: http://github.com/martinfinke/reaction
license: MIT
minimumCppStandard: 11
END_JUCE_MODULE_DECLARATION
*/
#endif

#pragma once

#include "util/internal/concurrentqueue.h"

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_events/juce_events.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include <atomic>
#include <exception>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <map>
#include <memory>
#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>

// Enable stricter warnings
#include "util/internal/reaction_ExtraWarnings.h"
#pragma clang diagnostic push
REACTION_ENABLE_EXTRA_WARNINGS

namespace reaction {
/// Used for Observables that don't emit a meaningful value, and just notify that something has changed.
typedef std::tuple<> Empty;

#include "util/internal/reaction_any.h"
#include "rx/reaction_Subscription.h"
#include "rx/reaction_DisposeBag.h"
#include "rx/internal/reaction_Observer_Impl.h"
#include "rx/reaction_Observer.h"
#include "rx/reaction_Scheduler.h"
#include "rx/internal/reaction_Observable_Impl.h"
#include "rx/reaction_Observable.h"
#include "rx/internal/reaction_Subjects_Impl.h"
#include "rx/reaction_Subjects.h"

#include "util/reaction_LockFreeSource.h"
#include "util/reaction_LockFreeTarget.h"

#include "integration/reaction_GUIExtensions.h"
#include "integration/reaction_ModelExtensions.h"
#include "integration/reaction_Reactive.h"
#include "integration/reaction_ReactiveGUI.h"
#include "integration/reaction_ReactiveModel.h"
}

#pragma clang diagnostic pop
