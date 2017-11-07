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

// Use stricter warnings (basically all except for a few):
#pragma clang diagnostic warning "-Weverything"
#pragma clang diagnostic ignored "-Wc++98-compat"
#pragma clang diagnostic ignored "-Wc++98-compat-pedantic"
#pragma clang diagnostic ignored "-Wshadow"
#pragma clang diagnostic ignored "-Wshadow-ivar"
#pragma clang diagnostic ignored "-Wpadded"
#pragma clang diagnostic ignored "-Wswitch-enum"
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wundef"
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#pragma clang diagnostic ignored "-Wweak-vtables"
#pragma clang diagnostic ignored "-Wdouble-promotion"
#pragma clang diagnostic ignored "-Wfloat-equal"

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
