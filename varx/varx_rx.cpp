#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcomma"
#include "RxCpp/Rx/v2/src/rxcpp/rx.hpp"
#pragma clang diagnostic pop

// Use stricter warnings (basically all except for a few). Because varx.h is not #included here, these need to be defined again.
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
