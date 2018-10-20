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
#include "common.h"

using namespace std;
using chrono::steady_clock;
using std::make_pair;

// Protocol: <0x01> <SEQ 4 bytes> <Content>
//           <0x02> <SEQ 4 bytes> -- means ACK

void PerfectLink::onMessage(unsigned source, char *buf, unsigned len)
{
    // parsing messages
    if(len < 4) return;

    // receiving an ACK from a sent message
    if(len == 5 && buf[0] == 0x02)
    {
        int seqnumack = charsToInt32(buf + 1);
        //cout << "** Received ACK " << seqnumack << endl;

        mtx.lock();
        free(msgs[seqnumack].second);
        msgs.erase(seqnumack);
        mtx.unlock();

    } // receiving content from another process => we send an ACK
    else if(buf[0] == 0x01) {
        //int tmp = charsToInt32(buf + 1);
        //cout << "** Received content " << tmp << endl;
        deliverToAll(source, buf + 5, len - 5);
        char sdata[5];
        sdata[0] = 0x02;
        memcpy(sdata + 1, buf + 1, 4);
        //cout << "** Sending ACK for " << tmp << endl;
        s->send(sdata, 5);
    }
}

void *PerfectLink::sendLoop(void *arg)
{
    // pointer to failure detector
    PerfectLink* link = (PerfectLink*) arg;
    
    // Sending loop
    while(true){
        
        if(link->msgs.size() > 0){
            // Send all messages if ACK missing
            map<int, pair<int, char*> >::iterator it;
            
            // start of critical section
            link->mtx.lock();
            for (it = link->msgs.begin(); it != link->msgs.end(); it++)
            {
                // seq number
                int tmp = (*it).first;

                // data
                char* sdata = (*it).second.second;

                // data length
                int len = (*it).second.first;

                // sending message
                link->s->send(sdata, len);
            }

            // end of critical section
            link->mtx.unlock();
        
            link->waitForAcksOrTimeout();
        }
    }
    
}


PerfectLink::PerfectLink(Sender *s, Receiver *r, Target *target) :
    Sender(s->getTarget()), Receiver(r->getThis(), target)
{
    r->addTarget(this);
    this->s = s;
    this->r = r;
    this->seqnum = 0;
    
    // starting sending thread
    pthread_create(&thread, nullptr, &PerfectLink::sendLoop, this);
    
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
void PerfectLink::send(char* buffer, int length)
{
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
    
    // saving the message
    this->msgs[this->seqnum] = make_pair(length + 5, data);
    
    // IMPORTANT: incrementing the sequence number
    // Otherwise another thread could send a message with SAME
    // sequence number
    this->seqnum++;
    
    // finished critical section
    mtx.unlock();
}

void PerfectLink::waitForAcksOrTimeout()
{
    // waiting until all messages are sent
    steady_clock::time_point begin = steady_clock::now();
    while(this->msgs.size() != 0 ) {
        long a = chrono::duration_cast<chrono::microseconds>(steady_clock::now() - begin).count();
        if (a > TIMEOUT) break;

        /// Sleep 1 millisecond
        /// Drastically reduces CPU load (thread sleep instead of busy wait)
        usleep(1000);
    }
}
