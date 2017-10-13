#pragma once

namespace detail {
class ReleasePool : private juce::Timer
{
public:
    static ReleasePool& get();
    void add(const std::shared_ptr<void>& item);
    size_t size() const;
    void cleanup();

private:
    std::vector<std::shared_ptr<void>> pool;
    juce::CriticalSection criticalSection;

    ReleasePool();
    void timerCallback() override;
    static bool isUnused(const std::shared_ptr<void>& item);
};
}
