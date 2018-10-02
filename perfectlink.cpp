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
    // repeat until no more messages to send
    for(int i=0; i< (int)(this->m/this->window); i++){
        // fill the list of messages to send
        fillMsgs();
        
        // send all messages of the list to dst
        std::map<int, char *>::iterator it;
        for (it = this->msgs.begin(); it != this->msgs.end(); it++) {
            char* data = (*it).second;
            this->s->send(data, MSG_SIZE + 4);
        }
    }
}

void PerfectLink::onMessage()
{
    
}

void PerfectLink::fillMsgs()
{
    int lb = this->seqnum;
    int ub = this->seqnum + this->window;
    
    // if window does not divide m, stop at m
    for(int i = lb; i < ub && i < this->m; i=i+1) {
        std::cout << i << std::endl;

        // allocating new memory
        char* data = (char*) malloc(MSG_SIZE + 4);
        
        // adding the sequence number
        int32ToChars(this->seqnum, data);

        // adding the message to the list
        this->msgs[this->seqnum] = data;
        this->seqnum++;
    }
}
