#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcomma"
#include "RxCpp/Rx/v2/src/rxcpp/rx.hpp"
#pragma clang diagnostic pop

// Enable stricter warnings
#include "util/internal/varx_ExtraWarnings.h"
#pragma clang diagnostic push
VARX_ENABLE_EXTRA_WARNINGS

#pragma clang diagnostic push
VARX_ENABLE_EXTRA_WARNINGS

namespace varx {
using namespace juce;

#include "util/internal/varx_any.h"
    
#include "rx/varx_Subscription.h"
#include "rx/internal/varx_Observable_Impl.h"
#include "rx/varx_Scheduler.h"
#include "rx/internal/varx_Observer_Impl.h"
#include "rx/internal/varx_Scheduler_Impl.h"
#include "rx/internal/varx_Subjects_Impl.h"
#include "rx/varx_DisposeBag.h"
#include "rx/varx_Subscription.cpp"
#include "rx/varx_DisposeBag.cpp"
#include "rx/varx_Scheduler.cpp"
#include "rx/internal/varx_Scheduler_Impl.cpp"
#include "rx/internal/varx_Observable_Impl.cpp"
#include "rx/internal/varx_Observer_Impl.cpp"
#include "rx/internal/varx_Subjects_Impl.cpp"
}

#pragma clang diagnostic pop
