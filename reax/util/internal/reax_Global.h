#pragma once

// Enables stricter warnings on LLVM/Mac (basically all available warnings except for a few)
#if JUCE_MAC
#define REAX_ENABLE_EXTRA_WARNINGS _Pragma("clang diagnostic warning \"-Weverything\"") _Pragma("clang diagnostic ignored \"-Wc++98-compat\"") _Pragma("clang diagnostic ignored \"-Wc++98-compat-pedantic\"") _Pragma("clang diagnostic ignored \"-Wshadow\"") _Pragma("clang diagnostic ignored \"-Wshadow-ivar\"") _Pragma("clang diagnostic ignored \"-Wpadded\"") _Pragma("clang diagnostic ignored \"-Wswitch-enum\"") _Pragma("clang diagnostic ignored \"-Wexit-time-destructors\"") _Pragma("clang diagnostic ignored \"-Wglobal-constructors\"") _Pragma("clang diagnostic ignored \"-Wundef\"") _Pragma("clang diagnostic ignored \"-Wdocumentation-unknown-command\"") _Pragma("clang diagnostic ignored \"-Wweak-vtables\"") _Pragma("clang diagnostic ignored \"-Wdouble-promotion\"") _Pragma("clang diagnostic ignored \"-Wfloat-equal\"") _Pragma("clang diagnostic warning \"-Weffc++\"")
#else
#define REAX_ENABLE_EXTRA_WARNINGS
#endif

namespace reax {
template<bool B, class T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

template<class T>
using decay_t = typename std::decay<T>::type;

template<bool B, class T, class F>
using conditional_t = typename std::conditional<B, T, F>::type;

template<class...>
struct conjunction : std::true_type
{
};
template<class B1>
struct conjunction<B1> : B1
{
};
template<class B1, class... Bn>
struct conjunction<B1, Bn...>
: conditional_t<bool(B1::value), conjunction<Bn...>, B1>
{
};
}
