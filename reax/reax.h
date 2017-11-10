#if 0
/*
BEGIN_JUCE_MODULE_DECLARATION
ID: reax
vendor: martin-finke
version: 0.7.0
name: ReaX
description: Reactive Extensions (Rx) for JUCE.
dependencies: juce_core, juce_data_structures, juce_events, juce_graphics, juce_gui_basics
website: http://github.com/martinfinke/reax
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
#include "util/internal/reax_ExtraWarnings.h"
#pragma clang diagnostic push
REAX_ENABLE_EXTRA_WARNINGS

namespace reax {
/// Used for Observables that don't emit a meaningful value, and just notify that something has changed.
typedef std::tuple<> Empty;

#include "util/internal/reax_any.h"
#include "rx/reax_Subscription.h"
#include "rx/reax_DisposeBag.h"
#include "rx/internal/reax_Observer_Impl.h"
#include "rx/reax_Observer.h"
#include "rx/reax_Scheduler.h"
#include "rx/internal/reax_Observable_Impl.h"
#include "rx/reax_Observable.h"
#include "rx/internal/reax_Subjects_Impl.h"
#include "rx/reax_Subjects.h"

#include "util/reax_LockFreeSource.h"
#include "util/reax_LockFreeTarget.h"

#include "integration/reax_GUIExtensions.h"
#include "integration/reax_ModelExtensions.h"
#include "integration/reax_Reactive.h"
#include "integration/reax_ReactiveGUI.h"
#include "integration/reax_ReactiveModel.h"
}

#pragma clang diagnostic pop
