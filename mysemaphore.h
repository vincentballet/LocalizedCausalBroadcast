/** @file mysemaphore.h
 * @author https://stackoverflow.com/questions/4792449/c0x-has-no-semaphores-how-to-synchronize-threads
 * @author Volodin Sergei, EPFL
 * @date Dec 2018
 */

#ifndef MYSEMAPHORE_H
#define MYSEMAPHORE_H

#include <mutex>
#include <condition_variable>

/// @class semaphore Implements a semaphore in C++
/// @see https://stackoverflow.com/questions/4792449/c0x-has-no-semaphores-how-to-synchronize-threads
class semaphore
{
private:
    std::mutex mutex;
    std::condition_variable condition;

    /// @brief The value of the semaphore
    unsigned long count = 0;

public:
    /**
     * @brief semaphore Initialize the semaphore with given count
     * @param count The initial value of the semaphore
     */
    semaphore(unsigned long count);

    /**
     * @brief notify Increase the value of the semaphore atomically and notify possible waiting threads
     */
    void notify();

    /**
     * @brief wait Decrement semaphore or wait until it's positive and then decrement
     */
    void wait();

    /**
     * @brief try_wait Decrement if possible, fail otherwise
     * @return True on success
     */
    bool try_wait();
};


#endif
