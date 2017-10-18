#pragma once

struct ObserverBase::Impl
{
    explicit Impl(const rxcpp::subscriber<var>& wrapped);

    const rxcpp::subscriber<var> wrapped;
};
