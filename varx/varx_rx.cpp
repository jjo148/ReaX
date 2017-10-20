#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>

#include "RxCpp/Rx/v2/src/rxcpp/rx.hpp"

namespace varx {
using namespace juce;
#warning Duplicate declaration, should be somewhere else
typedef std::exception_ptr Error;

#include "util/internal/varx_any.h"
    
#include "rx/varx_Disposable.h"
#include "rx/internal/varx_Observable_Impl.h"
#include "rx/varx_Scheduler.h"
#include "rx/internal/varx_Observer_Impl.h"
#include "rx/internal/varx_Scheduler_Impl.h"
#include "rx/internal/varx_Subjects_Impl.h"
#include "rx/varx_DisposeBag.h"
#include "rx/varx_Disposable.cpp"
#include "rx/varx_DisposeBag.cpp"
#include "rx/varx_Scheduler.cpp"
#include "rx/internal/varx_Scheduler_Impl.cpp"
#include "rx/internal/varx_Observable_Impl.cpp"
#include "rx/internal/varx_Observer_Impl.cpp"
#include "rx/internal/varx_Subjects_Impl.cpp"
}
