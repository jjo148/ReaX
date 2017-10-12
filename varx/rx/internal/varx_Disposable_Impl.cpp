#include "varx_Disposable_Impl.h"

Disposable::Impl::Impl(const rxcpp::subscription& wrapped)
: wrapped(wrapped) {}
