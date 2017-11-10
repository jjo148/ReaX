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
    void insert(Subscription&& subscription);

private:
    const detail::any wrapped;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DisposeBag)
};
