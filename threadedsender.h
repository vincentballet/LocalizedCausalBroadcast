/** @file threadedsender.h
 * @brief Decorator which adds a send queue
 * @author Volodin Sergei, EPFL
 * @date November 2018
 */

#ifndef THREADEDSENDER_H
#define THREADEDSENDER_H

#include "sender.h"
#include <pthread.h>
#include <semaphore.h>
#include <mutex>
#include <list>

using std::list;
using std::string;
using std::mutex;

/** @class Adds a send queue and a send thread */
class ThreadedSender : public Sender
{
private:
    /// @brief Send queue
    list<string> queue;

    /// @brief Send mutex
    mutex mtx;

    /// @brief Semaphore with number of items in the queue
    sem_t fill_sem;

    /// @brief Send thread
    pthread_t send_thread;

    /**
     * @brief sendLoop Launches the sending loop
     * @param arg ThreadedSender argument (this)
     * @return nullptr, never
     */
    static void* sendLoop(void* arg);

    /// @brief The object which actually sends data
    Sender* underlying_sender;
public:
    ThreadedSender(Sender* underlying_sender);
    virtual ~ThreadedSender() {}

    /**
     * @brief send array of bytes
     * @param data array of bytes
     * @param N length of the array
     */
    virtual void send(const char* data, unsigned N);
};

#endif // THREADEDSENDER_H
