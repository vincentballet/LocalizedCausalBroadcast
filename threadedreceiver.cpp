#include "threadedreceiver.h"

void *ThreadedReceiver::deliverLoop(void *arg)
{
    ThreadedReceiver* receiver = (ThreadedReceiver*) arg;

    while(true)
    {
        // waiting for data
        sem_wait(&(receiver->fill_sem));

        // locking the buffer
        receiver->m.lock();

        // found something to deliver?
        bool found = false;

        // buffer
        queued_message msg;

        // loop over queue
        if(!receiver->messages.empty())
        {
            found = true;

            // copying data
            msg = *(receiver->messages.begin());

            // removing first element
            receiver->messages.pop_front();
        }

        // now data is in msg, so can unlock the buffer
        receiver->m.unlock();

        // delivering messages, if any
        if(found)
        {
            vector<Target*>::iterator it;
            if(msg.logical_source == 0) for(it = receiver->targets.begin(); it != receiver->targets.end(); it++)
                (*it)->onMessage(msg.source, (char*) msg.data.c_str(), msg.data.length());
            else for(it = receiver->targets.begin(); it != receiver->targets.end(); it++)
                (*it)->onMessage(msg.source, msg.logical_source, (char*) msg.data.c_str(), msg.data.length());
        }
    }

    // never returns
    return nullptr;
}

void ThreadedReceiver::deliverToAll(unsigned source, unsigned logical_source, char *message, unsigned length)
{
    // WARNING: set size is unbounded!

    m.lock();

    // adding message to the buffer
    queued_message buf;
    buf.data = string(message, length);
    buf.source = source;
    buf.logical_source = logical_source;
    messages.push_back(buf);

    m.unlock();

    // +1 to the semaphore
    sem_post(&fill_sem);
}

void ThreadedReceiver::deliverToAll(unsigned source, char *message, unsigned length)
{
    // deliver to all with three arguments
    deliverToAll(source, 0, message, length);
}

ThreadedReceiver::ThreadedReceiver(int this_process, Target *target) : Receiver(this_process, target)
{
    // creating the delivery thread
    pthread_create(&deliver_thread, nullptr, &ThreadedReceiver::deliverLoop, this);

    // creating the fill semaphore
    sem_init(&fill_sem, 0, 0);
}
