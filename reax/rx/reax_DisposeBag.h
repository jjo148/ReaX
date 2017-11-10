#pragma once

class Subscription;

/**
    Disposes added `Subscription​`s when it is destroyed.
 */
class DisposeBag
{
public:
    /// Creates a new, empty `DisposeBag`.
    DisposeBag();

    /// Disposes all inserted `Subscription`​s in the `DisposeBag`.
    ~DisposeBag();

    /// Inserts a `Subscription` into the `DisposeBag`. The `Subscription` is disposed when the `DisposeBag` is destroyed. 
    void insert(const Subscription& subscription);

private:
    detail::any wrapped;

    JUCE_LEAK_DETECTOR(DisposeBag)
};
