#define DONT_SET_USING_JUCE_NAMESPACE 1
#include "JuceHeader.h"

// Enable stricter warnings
#include "util/internal/reax_Global.h"
#pragma clang diagnostic push
REAX_ENABLE_EXTRA_WARNINGS

namespace reax {
using namespace juce;
    
#include "integration/reax_GUIExtensions.cpp"
#include "integration/reax_ModelExtensions.cpp"
#include "integration/reax_ReactiveModel.cpp"

#include "util/internal/reax_any.cpp"
}

#pragma clang diagnostic pop
