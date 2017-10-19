#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>

#include "RxCpp/Rx/v2/src/rxcpp/rx.hpp"

namespace varx {
using namespace juce;
#warning Duplicate declaration, should be somewhere else
typedef std::exception_ptr Error;

#include "util/internal/varx_any.h"
    
#include "rx/varx_Disposable.h"
#include "rx/varx_DisposeBag.h"
#include "rx/internal/varx_Disposable_Impl.h"
#include "rx/internal/varx_DisposeBag_Impl.h"
#include "rx/varx_Observable.h"
#include "rx/varx_Scheduler.h"
#include "rx/varx_Observer.h"
#include "rx/internal/varx_Scheduler_Impl.h"
#include "rx/varx_Subjects.h"
#include "rx/varx_Disposable.cpp"
#include "rx/varx_DisposeBag.cpp"
#include "rx/varx_Observable.cpp"
#include "rx/varx_Observer.cpp"
#include "rx/varx_Scheduler.cpp"
#include "rx/varx_Subjects.cpp"
#include "rx/internal/varx_Disposable_Impl.cpp"
#include "rx/internal/varx_Scheduler_Impl.cpp"
}
