#include "varx_ReleasePool.h"

namespace detail {
ReleasePool& ReleasePool::get()
{
    static ReleasePool pool;
    return pool;
}

ReleasePool::ReleasePool()
{
    startTimer(1000);
}

void ReleasePool::add(const std::shared_ptr<void>& item)
{
    if (!item)
        return;

    const ScopedLock lock(criticalSection);

    // Don't insert the same item twice
    if (std::find(pool.begin(), pool.end(), item) != pool.end())
        return;

    pool.emplace_back(item);
    cleanup();
}

size_t ReleasePool::size() const
{
    const ScopedLock lock(criticalSection);
    return pool.size();
}

void ReleasePool::cleanup()
{
    const ScopedLock lock(criticalSection);
    pool.erase(std::remove_if(pool.begin(), pool.end(), isUnused),
               pool.end());
}

void ReleasePool::timerCallback()
{
    cleanup();
}

bool ReleasePool::isUnused(const std::shared_ptr<void>& item)
{
    // An item is unused if it's not referenced by anything except for this pool
    return (item.use_count() <= 1);
}
}
