//
//  perfectlink.cpp
//  DA_PaymentSystem
//
//  Created by Vincent Ballet on 02/10/2018.
//  Copyright © 2018 Vincent Ballet. All rights reserved.
//

#include "perfectlink.h"
#include "common.h"
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <mutex>
#include <cstring>
#include <chrono>

using namespace std;
using chrono::steady_clock;

/** @brief ??? */
unsigned const MSG_SIZE = 1000;

/** @brief ??? */
unsigned const BUF_SIZE = 1000;

/** @brief ??? */
unsigned const TIMEOUT = 5000000; // in microseconds (1e-6 sec)

/** @brief A mutex to be used by the PerfectLink class
 * @todo Use different mutexes for different PerfectLinks to allow concurrent receiving for example */
mutex mtx;

void PerfectLink::onMessage(unsigned source, char *buf, unsigned len)
{
    // receiving an ACK from a sent message
    if(len == 7 && memmem(buf + 4, 3, "ACK", 3))
    {
        int seqnumack = charsToInt32(buf);
        cout << "** Received ACK " << seqnumack << endl;

        mtx.lock();
        msgs.erase(seqnumack);
        mtx.unlock();

    } // receiving content from another process => we send an ACK
    else {
        int tmp = charsToInt32(buf);
        cout << "** Received content " << tmp << endl;
        if(target)
            target->onMessage(source, buf, len);
        char* sdata = static_cast<char*>(malloc(7));
        memcpy(sdata, buf, 4);
        memcpy(sdata + 4, "ACK", 3);
        cout << "** Sending ACK for " << tmp << endl;
        s->send(sdata, 7);
        free(sdata);
    }
}

PerfectLink::PerfectLink(Sender *s, Receiver *r, Target *target) :
    Sender(s->getTarget()), Receiver(r->getThis(), target)
{
    r->setTarget(this);
    this->s = s;
    this->r = r;
    this->seqnum = 0;
    this->window = 10;
}

void PerfectLink::send(char* buffer, int length)
{
    craftAndStoreMsg();

    // Send all messages if ACK missing
    map<int, char*>::iterator it;
    mtx.lock();
    for (it = this->msgs.begin(); it != this->msgs.end(); it++) {
        int tmp = (*it).first;
        char* sdata = (*it).second;
        s->send(sdata, MSG_SIZE + 4);
        cout << "> Sending " << tmp << endl;
    }
    mtx.unlock();
    waitForAcksOrTimeout();
}

void PerfectLink::waitForAcksOrTimeout(){
    steady_clock::time_point begin = steady_clock::now();
    while(this->msgs.size() != 0 ) {
        long a = chrono::duration_cast<chrono::microseconds>(steady_clock::now() - begin).count();
        if (a > TIMEOUT) break;

        /// Sleep 1 millisecond
        /// Drastically reduces CPU load (thread sleep instead of busy wait)
        usleep(1000);
    }
}

void PerfectLink::craftAndStoreMsg(){
    // allocating new memory
    /// @todo Where is it freed? Is there a memory leak?
    char* data = static_cast<char*>(malloc(MSG_SIZE + 4));
    
    // adding the sequence number
    int32ToChars(this->seqnum, data);

    // adding the message to the list
    mtx.lock();
    this->msgs[this->seqnum] = data;

    // IMPORTANT: incrementing the sequence number
    // Otherwise another thread could send a message with SAME
    // sequence number
    this->seqnum++;
    mtx.unlock();
}
