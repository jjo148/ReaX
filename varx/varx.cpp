#define DONT_SET_USING_JUCE_NAMESPACE 1
#include "JuceHeader.h"

// Enable stricter warnings
#pragma clang diagnostic push
VARX_ENABLE_EXTRA_WARNINGS

namespace varx {
using namespace juce;
    
#include "integration/varx_GUIExtensions.cpp"
#include "integration/varx_ModelExtensions.cpp"
#include "integration/varx_ReactiveModel.cpp"

#include "util/internal/varx_any.cpp"
#include "util/internal/varx_ReleasePool.cpp"
}

#pragma clang diagnostic pop
