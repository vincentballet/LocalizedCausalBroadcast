#include "failuredetector.h"
#include <cstring>
#include <pthread.h>
#include <unistd.h>
#include <iostream>

using std::cerr;
using std::endl;

void FailureDetector::onMessage(unsigned source, char *buffer, unsigned length)
{
    // checking if the source is correct
    if(source != s->getTarget()) return;

    // length must be 1
    if(length != 1) return;

    if(buffer[0] == (char) first_byte_ping)
    {
        char buffer1[1];
        buffer1[0] = (char) first_byte_pong;
        s->send(buffer1, 1);
    }

    // in any case, registering the response
    last_reply = std::chrono::duration_cast<std::chrono::milliseconds>(steady_clock::now().time_since_epoch()).count();
}

void *FailureDetector::pingLoop(void *arg)
{
    // pointer to failure detector
    FailureDetector* detector = (FailureDetector*) arg;

    // have reported failure already?
    bool reported = false;

    // interval for ping in ms
    int ping_interval = detector->timeout_ms / 3;

    // current time
    long begin = std::chrono::duration_cast<std::chrono::milliseconds>(steady_clock::now().time_since_epoch()).count();

    // forever
    while(true)
    {
        // getting current time
        long now = std::chrono::duration_cast<std::chrono::milliseconds>(steady_clock::now().time_since_epoch()).count();

        // checking if it's time to PING
        long delta = now - begin;
        if (delta > ping_interval)
        {
            //cerr << "Ping" << endl;
            char buf[1];
            buf[0] = (char) first_byte_ping;
            detector->s->send(buf, 1);
            begin = std::chrono::duration_cast<std::chrono::milliseconds>(steady_clock::now().time_since_epoch()).count();
        }

        // checking if interval since last reply is too big
        long delta_fail = now - detector->last_reply;
        if(delta_fail > detector->timeout_ms && !reported)
        {
            // reporting failure (only once)
            if(detector->monitor)
            {
                detector->monitor->onFailure(detector->s->getTarget());
                reported = true;
            }
        }

        // Sleep 1 millisecond
        usleep(1000);
    }
}

FailureDetector::FailureDetector(Sender *s, Receiver* r, int timeout_ms, FailureMonitor *monitor)
{
    r->addTarget(this);
    // saving parameters
    this->monitor = monitor;
    this->s = s;
    this->timeout_ms = timeout_ms;

    // initializing last_reply
    last_reply = std::chrono::duration_cast<std::chrono::milliseconds>(steady_clock::now().time_since_epoch()).count();

    // starting ping thread
    pthread_create(&thread, nullptr, &FailureDetector::pingLoop, this);
}

void FailureDetector::setMonitor(FailureMonitor *monitor)
{
    this->monitor = monitor;
}
