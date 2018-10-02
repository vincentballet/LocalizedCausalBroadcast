//
//  perfectlink.cpp
//  DA_PaymentSystem
//
//  Created by Vincent Ballet on 02/10/2018.
//  Copyright © 2018 Vincent Ballet. All rights reserved.
//

#include "perfectlink.h"
#include "common.h"
#include <pthread.h>
#include <iostream>

unsigned const MSG_SIZE = 1000;
unsigned const BUF_SIZE = 10000;

PerfectLink::PerfectLink(UDPSender* s, UDPReceiver* r, int m)
{
    this->s = s;
    this->r = r;
    this->m = m;
    this->seqnum = 0;
    this->window = 10;
}

void PerfectLink::send()
{
    // buffer for receiving messages
    char buf[BUF_SIZE];

    // repeat until list len < 10
    // TODO Stopping criterion ?
    while(true){
        if (this->msgs.size() < this->window && this->seqnum < this->m){
            // TODO Locks ?
            char* sdata = craftAndStoreMsg();
            this->s->send(sdata, MSG_SIZE + 4);
        }
    }
    
    bool locksimplemented = false;
    if (locksimplemented){
        char* rdata = (char*) malloc(MSG_SIZE + 4);
        while(true){
            int len = r->receive(buf, MSG_SIZE);
            // receiving an ACK from sent message
            if(len == 7 && memmem(buf, 4, rdata, 4) && memmem(buf + 4, 3, "ACK", 3))
            {
                // TODO Locks ?
                int seqnumack = charsToInt32(rdata);
                this->msgs.erase(seqnumack);

                break;
            } // receiving content from another process => we send an ACK
            else {
                char* sdata = (char*) malloc(7);
                memcpy(sdata, buf, 4);
                memcpy(sdata + 4, "ACK", 3);
                this->s->send(sdata, 7);
            }
        }
    }
    
}

void PerfectLink::onMessage()
{
    
}


char * PerfectLink::craftAndStoreMsg(){
    // allocating new memory
    char* data = (char*) malloc(MSG_SIZE + 4);
    
    // adding the sequence number
    int32ToChars(this->seqnum, data);

    // adding the message to the list
    this->msgs[this->seqnum] = data;

    this->seqnum++;

    return data;
}
