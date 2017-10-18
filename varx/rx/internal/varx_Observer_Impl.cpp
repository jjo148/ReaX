#include "varx_Observer_Impl.h"

ObserverBase::Impl::Impl(const rxcpp::subscriber<var>& wrapped)
: wrapped(wrapped) {}
