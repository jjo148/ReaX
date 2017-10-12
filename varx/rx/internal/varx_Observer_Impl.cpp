#include "varx_Observer_Impl.h"

Observer::Impl::Impl(const rxcpp::subscriber<var>& wrapped)
: wrapped(wrapped) {}
