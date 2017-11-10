// Enables stricter warnings on LLVM/Mac (basically all available warnings except for a few)
#if JUCE_MAC
#define REACTION_ENABLE_EXTRA_WARNINGS _Pragma("clang diagnostic warning \"-Weverything\"") _Pragma("clang diagnostic ignored \"-Wc++98-compat\"") _Pragma("clang diagnostic ignored \"-Wc++98-compat-pedantic\"") _Pragma("clang diagnostic ignored \"-Wshadow\"") _Pragma("clang diagnostic ignored \"-Wshadow-ivar\"") _Pragma("clang diagnostic ignored \"-Wpadded\"") _Pragma("clang diagnostic ignored \"-Wswitch-enum\"") _Pragma("clang diagnostic ignored \"-Wexit-time-destructors\"") _Pragma("clang diagnostic ignored \"-Wglobal-constructors\"") _Pragma("clang diagnostic ignored \"-Wundef\"") _Pragma("clang diagnostic ignored \"-Wdocumentation\"") _Pragma("clang diagnostic ignored \"-Wdocumentation-unknown-command\"") _Pragma("clang diagnostic ignored \"-Wweak-vtables\"") _Pragma("clang diagnostic ignored \"-Wdouble-promotion\"") _Pragma("clang diagnostic ignored \"-Wfloat-equal\"")
#else
#define REACTION_ENABLE_EXTRA_WARNINGS
#endif
