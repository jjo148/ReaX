#include "JuceHeader.h"

namespace varx {
using namespace juce;
    
#include "integration/varx_ExtensionBase.cpp"
#include "integration/varx_GUIExtensions.cpp"
#include "integration/varx_ModelExtensions.cpp"
#include "integration/varx_Reactive.cpp"
#include "integration/varx_ReactiveGUI.cpp"
#include "integration/varx_ReactiveModel.cpp"

#include "util/internal/varx_any.cpp"
#include "util/internal/varx_ReleasePool.cpp"
#include "util/varx_LockFreeSource.cpp"
#include "util/varx_PrintFunctions.cpp"
#include "util/varx_VariantConverters.cpp"
}
