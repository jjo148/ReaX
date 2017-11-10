#define DONT_SET_USING_JUCE_NAMESPACE 1
#include "JuceHeader.h"

// Enable stricter warnings
#pragma clang diagnostic push
REACTION_ENABLE_EXTRA_WARNINGS

namespace reaction {
using namespace juce;
    
#include "integration/reaction_GUIExtensions.cpp"
#include "integration/reaction_ModelExtensions.cpp"
#include "integration/reaction_ReactiveModel.cpp"

#include "util/internal/reaction_any.cpp"
}

#pragma clang diagnostic pop
