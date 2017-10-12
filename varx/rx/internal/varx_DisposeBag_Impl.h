#pragma once

struct DisposeBag::Impl
{
    const rxcpp::composite_subscription wrapped;
};
