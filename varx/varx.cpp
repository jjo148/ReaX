#include "JuceHeader.h"

#include "RxCpp/Rx/v2/src/rxcpp/rx.hpp"

namespace varx {
using namespace juce;

typedef rxcpp::subscriber<var> rxcpp_subscriber;

template<class T>
using shared_ptr = std::shared_ptr<T>;

#include "integration/varx_ExtensionBase.cpp"
#include "integration/varx_GUIExtensions.cpp"
#include "integration/varx_ModelExtensions.cpp"
#include "integration/varx_Reactive.cpp"
#include "integration/varx_ReactiveGUI.cpp"
#include "integration/varx_ReactiveModel.cpp"

#include "rx/internal/varx_Disposable_Impl.cpp"
#include "rx/internal/varx_DisposeBag_Impl.h"
#include "rx/internal/varx_Observable_Impl.cpp"
#include "rx/internal/varx_Scheduler_Impl.cpp"
#include "rx/internal/varx_Subjects_Impl.cpp"

#include "rx/varx_Disposable.cpp"
#include "rx/varx_DisposeBag.cpp"
#include "rx/varx_Observable.cpp"
#include "rx/varx_Observer.cpp"
#include "rx/varx_Scheduler.cpp"
#include "rx/varx_Subjects.cpp"

#include "util/internal/varx_any.cpp"
#include "util/internal/varx_ReleasePool.cpp"
#include "util/varx_LockFreeSource.cpp"
#include "util/varx_PrintFunctions.cpp"
#include "util/varx_VariantConverters.cpp"
}
