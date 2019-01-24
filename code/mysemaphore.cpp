#include "mysemaphore.h"


semaphore::semaphore(unsigned long count): count(count)
{
}

void semaphore::notify() {
    std::lock_guard<decltype(mutex)> lock(mutex);
    ++count;
    condition.notify_one();
}

void semaphore::wait() {
    std::unique_lock<decltype(mutex)> lock(mutex);
    while(!count) // Handle spurious wake-ups.
        condition.wait(lock);
    --count;
}

bool semaphore::try_wait() {
    std::lock_guard<decltype(mutex)> lock(mutex);
    if(count) {
        --count;
        return true;
    }
    return false;
}
