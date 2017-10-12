#pragma once

struct Disposable::Impl
{
    Impl(const rxcpp::subscription& wrapped);

    const rxcpp::subscription wrapped;
};
