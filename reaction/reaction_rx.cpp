#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcomma"
#include "RxCpp/Rx/v2/src/rxcpp/rx.hpp"
#pragma clang diagnostic pop

// Enable stricter warnings
#include "util/internal/reaction_ExtraWarnings.h"
#pragma clang diagnostic push
REACTION_ENABLE_EXTRA_WARNINGS

namespace reaction {
using namespace juce;

#include "util/internal/reaction_any.h"
    
#include "rx/reaction_Subscription.h"
#include "rx/internal/reaction_Observable_Impl.h"
#include "rx/reaction_Scheduler.h"
#include "rx/internal/reaction_Observer_Impl.h"
#include "rx/internal/reaction_Scheduler_Impl.h"
#include "rx/internal/reaction_Subjects_Impl.h"
#include "rx/reaction_DisposeBag.h"
#include "rx/reaction_Subscription.cpp"
#include "rx/reaction_DisposeBag.cpp"
#include "rx/reaction_Scheduler.cpp"
#include "rx/internal/reaction_Scheduler_Impl.cpp"
#include "rx/internal/reaction_Observable_Impl.cpp"
#include "rx/internal/reaction_Observer_Impl.cpp"
#include "rx/internal/reaction_Subjects_Impl.cpp"
}

#pragma clang diagnostic pop
