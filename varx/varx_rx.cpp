#include "JuceHeader.h"
#include "RxCpp/Rx/v2/src/rxcpp/rx.hpp"

namespace varx {
using namespace juce;

#include "rx/internal/varx_Disposable_Impl.cpp"
#include "rx/internal/varx_DisposeBag_Impl.h"
#include "rx/internal/varx_Scheduler_Impl.cpp"

#include "rx/varx_Disposable.cpp"
#include "rx/varx_DisposeBag.cpp"
#include "rx/varx_Observable.cpp"
#include "rx/varx_Observer.cpp"
#include "rx/varx_Scheduler.cpp"
#include "rx/varx_Subjects.cpp"
}
