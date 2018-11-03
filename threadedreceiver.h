/** @file threadedreceiver.h
 * @brief This file implements a receiver which delivers
 * its messages in a separate thread
 * @author Volodin Sergei, EPFL
 * @date November 2018
 */

#ifndef THREADEDRECEIVER_H
#define THREADEDRECEIVER_H

#include <list>
#include <mutex>
#include <string>
#include "receiver.h"
#include <pthread.h>
#include <semaphore.h>

using std::list;
using std::string;
using std::mutex;

/** @class This file implements a receiver which delivers
 * its messages in a separate thread */
class ThreadedReceiver : public Receiver
{
private:
    /// @brief Buffer data
    struct queued_message {
        /// @brief Message
        string data;

        /// @brief Source of the message
        unsigned source;

        /// @brief Logical source (can be 0)
        /// 0 means that three argument function is used
        unsigned logical_source;
    };

    /// @brief fill semaphore
    /// Indicates the number of entries inside the messages buffer
    /// @see https://en.wikipedia.org/wiki/Producer%E2%80%93consumer_problem
    sem_t fill_sem;

    /// @brief Buffer for delivery
    list<queued_message> messages;

    /// @brief Thread in which delivery occurs
    pthread_t deliver_thread;

    /// @brief Mutex for the buffer
    mutex m;

    /**
     * @brief deliverLoop Loop for delivering messages
     * @param arg The ThreadedReciever instance (this)
     * @return nullptr (never returns)
     */
    static void* deliverLoop(void* arg);

protected:
    /// @brief Deliver a message to all targets with logical source
    void deliverToAll(unsigned source, unsigned logical_source, const char* queued_message, unsigned length);

    /// @brief Deliver a message to all targets
    void deliverToAll(unsigned source, const char* queued_message, unsigned length);
public:
    /**
     * @brief Receiver constructor
     * @param target The object to deliver messages to
     */
    ThreadedReceiver(unsigned this_process, Target* target = nullptr);

    virtual ~ThreadedReceiver() {}
};

#endif // THREADEDRECEIVER_H
