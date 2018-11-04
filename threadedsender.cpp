#include "threadedsender.h"

void *ThreadedSender::sendLoop(void *arg)
{
    // this for ThreadedSender
    ThreadedSender* sender = (ThreadedSender*) arg;

    // string to send
    string buffer;

    // does buffer contain any data?
    bool isFull = false;

    while(true)
    {
        // waiting for some data
        sem_wait(&(sender->fill_sem));

        // critical section
        sender->mtx.lock();

        // sending one of the objects
        isFull = sender->queue.size() > 0;
        if(isFull)
        {
            // taking last object
            buffer = sender->queue.back();

            // removing last object
            sender->queue.pop_back();
        }

        // end of critical section
        sender->mtx.unlock();

        // sending data actually
        if(isFull)
            sender->underlying_sender->send(buffer.c_str(),
                                            buffer.length());
    }
}

ThreadedSender::ThreadedSender(Sender *underlying_sender) :
    Sender(underlying_sender->getTarget()), underlying_sender(underlying_sender)
{
    // creating the fill semaphore
    sem_init(&fill_sem, 0, 0);

    // creating the delivery thread
    pthread_create(&send_thread, nullptr, &ThreadedSender::sendLoop, this);
}

void ThreadedSender::send(const char *data, unsigned N)
{
    // critical section
    mtx.lock();

    // saving data to the buffer
    queue.push_back(string(data, N));

    // end of critical section
    mtx.unlock();

    // +1 to the semaphore
    sem_post(&fill_sem);
}
