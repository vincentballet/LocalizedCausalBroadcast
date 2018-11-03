//
//  perfectlink.cpp
//  DA_PaymentSystem
//
//  Created by Vincent Ballet on 02/10/2018.
//  Copyright © 2018 Vincent Ballet. All rights reserved.
//

#include "perfectlink.h"
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <chrono>
#include <cassert>
#include <set>
#include "common.h"

using namespace std;
using chrono::steady_clock;
using std::make_pair;
using std::set;
using std::string;
using std::get;

// Protocol: <0x01> <SEQ 4 bytes> <Content>
//           <0x02> <SEQ 4 bytes> -- means ACK

void PerfectLink::onMessage(unsigned source, const char* buf, unsigned len)
{
    // ignoring foreign messages
    if(source != s->getTarget()) return;

    // parsing messages
    if(len < 5) return;

    // receiving an ACK from a sent message
    if(len == 5 && buf[0] == 0x02)
    {
        // sequence number which has been acked
        int seqnumack = charsToInt32(buf + 1);

#ifdef PERFECTLINK_DEBUG
        stringstream ss;
        ss << "< plack " << r->getThis() <<  " " << source << " " << charsToInt32(buf + 1); // << " " << charsToInt32(buf + 5 + 8);
        memorylog->log(ss.str());
#endif

        mtx.lock();
        if(msgs.find(seqnumack) != msgs.end())
        {
            free(get<1>(msgs[seqnumack]));
            msgs.erase(seqnumack);
            inqueue--;

            // +1 to the empty
            sem_post(&empty_sem);

            // -1 from the fill
            sem_wait(&fill_sem);
        } // else: ACKed an unknown message!
        mtx.unlock();

    } // receiving content from another process => we send an ACK
    else if(buf[0] == 0x01) {
        // creating message as a string
        string message(buf, len);

        char sdata[5];
        sdata[0] = 0x02;
        memcpy(sdata + 1, buf + 1, 4);
        //cout << "** Sending ACK for " << tmp << endl;
        s->send(sdata, 5);

        // need to deliver this message?
        bool need_deliver = false;

        // no need for critical section
        // because only one thread calls onMessage
        // which is the UDPReceiver thread
        //mtx.lock();

        // WARNING: need to return ONLY after sending the ACK, otherwise can lose the ACK
        // And the sender will keep sending this message
        if (delivered.find(message) == delivered.end())
        {
            need_deliver = true;
            delivered.insert(message);
        }
        //mtx.unlock();

        // delivering message
        if(need_deliver)
        {
#ifdef PERFECTLINK_DEBUG
            stringstream ss;
            ss << "< pld " << r->getThis() << " " << source << " " << charsToInt32(buf + 1); // << " " << charsToInt32(buf + 5 + 8);
            memorylog->log(ss.str());
#endif

            // will pass the data to a separate thread for processing!
            deliverToAll(source, buf + 5, len - 5);
        }
    }
}

void *PerfectLink::sendLoop(void *arg)
{
    // pointer to failure detector
    PerfectLink* link = (PerfectLink*) arg;

    // Iterator for msgs
    map<int, tuple<int, char*, long> >::iterator it;

    // Sending loop
    while(true)
    {
        // updading clean variable
        link->clean = link->msgs.size() == 0;

        // doing nothing if the link is not running anymore
        if(!link->running)
        {
            usleep(10000);
            continue;
        }

        // checking if there are messages in the queue
        sem_wait(&(link->fill_sem));
        sem_post(&(link->fill_sem));

        // start of critical section
        link->mtx.lock();

        // if no messages need to be sent now
        // this will be equal to the minimal time (in ms)
        // in which a message should be sent
        int64_t min_send_in = TIMEOUT_MSG;

        // loop over the buffer
        // if buffer is empty, will perform no iterations
        // and will wait on the semaphore at the next while() iteration
        for (it = link->msgs.begin(); it != link->msgs.end(); it++)
        {
            // data
            const char* sdata = get<1>((*it).second);

            // data length
            int len = get<0>((*it).second);

            // last sent timestamp
            int64_t last_sent = get<2>((*it).second);

            // in how many milliseconds the message should be sent?
            int64_t send_in = last_sent + TIMEOUT_MSG - TIME_MS_NOW();

            // calculatin min_send_in
            min_send_in = min(min_send_in, send_in);

            // if send_in is positive, it means that it's not
            // yet time to send this message
            if(send_in > 0) continue;

            // sending message
            link->s->send(sdata, len);

#ifdef PERFECTLINK_DEBUG
            // logging message
            stringstream ss;
            ss << "> pls " << TIME_MS_NOW() << " " << link->s->getTarget() << " " << link->r->getThis() << " " << charsToInt32(sdata + 1);// << " " << charsToInt32(sdata + 5 + 8);
            memorylog->log(ss.str());
#endif
            // filling in last_sent time
            get<2>((*it).second) = TIME_MS_NOW();
        }

        // end of critical section
        link->mtx.unlock();

        // sleeping until a next message needs to be sent
        if(min_send_in > 0)
            usleep(min_send_in * 1000);
    }
}

PerfectLink::PerfectLink(Sender *s, Receiver *r, Target *target) :
    Sender(s->getTarget()), ThreadedReceiver(r->getThis(), target)
{
    r->addTarget(this);
    this->s = s;
    this->r = r;
    this->seqnum = 0;

    // currently no messages inside the queue
    inqueue = 0;
    
    // initializing fill/empty semaphores
    sem_init(&fill_sem, 0, 0);
    sem_init(&empty_sem, 0, MAX_IN_QUEUE);

    // starting sending thread
    pthread_create(&send_thread, nullptr, &PerfectLink::sendLoop, this);
}

Sender *PerfectLink::getSender()
{
    return s;
}

Receiver *PerfectLink::getReceiver()
{
    return r;
}

/// @todo bad name, does not send but adds message to send list
void PerfectLink::send(const char* buffer, int length)
{
    // parameters check
    if (!(buffer && length > 0)){
        return;
    }

    // doing nothing if link is stopped
    if(!running) return;

    // waiting until can send
    // VIA -1 to the empty semaphore
    sem_wait(&empty_sem);

    // allocating new memory
    char* data = static_cast<char*>(malloc(length + 5));
    
    // adding the message to the list
    mtx.lock();
    
    // adding the sequence number
    int32ToChars(this->seqnum, data + 1);
    
    // filling in first byte
    data[0] = 0x01;
    
    // copying data
    memcpy(data + 5, buffer, length);
    
    // sending message NOW
    s->send(data, length + 5);

    // saving the message and current timestamp
    this->msgs[this->seqnum] = make_tuple(length + 5, data, TIME_MS_NOW());

    // IMPORTANT: incrementing the sequence number
    // Otherwise another thread could send a message with SAME
    // sequence number
    this->seqnum++;

    // +1 message in queue
    inqueue++;

    // +1 to the fill semaphore
    sem_post(&fill_sem);

    // finished critical section
    mtx.unlock();
}

void PerfectLink::halt()
{
#ifdef PERFECTLINK_DEBUG
    stringstream ss;
    ss << "Stopping link to " << s->getTarget();
    memorylog->log(ss.str());
#endif
    running = false;
}

bool PerfectLink::isClean()
{
    return clean;
}
