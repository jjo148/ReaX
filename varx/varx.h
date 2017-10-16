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

#include "util/internal/concurrentqueue.h"

#include <atomic>
#include <exception>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <map>
#include <memory>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>


#include "util/varx_PrintFunctions.h"
#include "util/varx_VariantConverters.h"

namespace varx {
    typedef std::exception_ptr Error;

#include "rx/varx_Disposable.h"
#include "rx/varx_DisposeBag.h"
#include "rx/varx_Observable.h"
#include "rx/varx_Observer.h"
#include "rx/varx_Scheduler.h"
#include "rx/varx_Subjects.h"
}

VARX_DEFINE_VARIANT_CONVERTER(varx::Observable)

namespace varx {
#include "util/internal/varx_ReleasePool.h"
#include "util/varx_LockFreeSource.h"
#include "util/varx_LockFreeTarget.h"
#include "integration/varx_ExtensionBase.h"
#include "integration/varx_GUIExtensions.h"
#include "integration/varx_ModelExtensions.h"
#include "integration/varx_Reactive.h"
#include "integration/varx_ReactiveGUI.h"
#include "integration/varx_ReactiveModel.h"
}
