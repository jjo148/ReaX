#pragma once

struct Observer::Impl
{
    explicit Impl(const rxcpp::subscriber<var>& wrapped);

    const rxcpp::subscriber<var> wrapped;
};
