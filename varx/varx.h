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
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>
#include "util/internal/concurrentqueue.h"

namespace varx {

struct Empty
{};
inline bool operator==(const Empty& lhs, const Empty& rhs)
{
    return true;
}
inline bool operator!=(const Empty& lhs, const Empty& rhs)
{
    return !(lhs == rhs);
}
}

namespace varx {
#include "util/internal/varx_any.h"
#include "rx/varx_Disposable.h"
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

#include "integration/varx_ExtensionBase.h"
#include "integration/varx_GUIExtensions.h"
#include "integration/varx_ModelExtensions.h"
#include "integration/varx_Reactive.h"
#include "integration/varx_ReactiveGUI.h"
#include "integration/varx_ReactiveModel.h"
}

#include "util/varx_VariantConverters.h"
#include "util/varx_PrintFunctions.h"

VARX_DEFINE_VARIANT_CONVERTER(varx::detail::ObservableImpl)
VARX_DEFINE_VARIANT_CONVERTER(varx::Empty)

template<typename... Args>
struct juce::VariantConverter<std::tuple<Args...>> : varx::detail::VariantConverter<std::tuple<Args...>>
{
};

template<typename T>
struct juce::VariantConverter<varx::Observable<T>> : varx::detail::VariantConverter<varx::Observable<T>>
{
};
