#if 0
/*
BEGIN_JUCE_MODULE_DECLARATION
ID: varx
vendor: martin-finke
version: 0.6.2
name: varx
description: Reactive Extensions (Rx) for JUCE.
dependencies: juce_core, juce_data_structures, juce_events, juce_graphics, juce_gui_basics
website: http://github.com/martinfinke/varx
license: MIT
minimumCppStandard: 11
END_JUCE_MODULE_DECLARATION
*/
#endif

#pragma once

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


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wextra-semi"
#include "util/internal/concurrentqueue.h"
#pragma clang diagnostic pop

namespace varx {
/// Used for Observables that don't emit a meaningful value, and just notify that something has changed.
typedef std::tuple<> Empty;

#include "util/internal/varx_any.h"
#include "rx/varx_Subscription.h"
#include "rx/varx_DisposeBag.h"
#include "rx/internal/varx_Observer_Impl.h"
#include "rx/varx_Observer.h"
#include "rx/varx_Scheduler.h"
#include "rx/internal/varx_Observable_Impl.h"
#include "rx/varx_Observable.h"
#include "rx/internal/varx_Subjects_Impl.h"
#include "rx/varx_Subjects.h"

#include "util/internal/varx_ReleasePool.h"
#include "util/varx_LockFreeSource.h"
#include "util/varx_LockFreeTarget.h"

#include "integration/varx_GUIExtensions.h"
#include "integration/varx_ModelExtensions.h"
#include "integration/varx_Reactive.h"
#include "integration/varx_ReactiveGUI.h"
#include "integration/varx_ReactiveModel.h"
}
