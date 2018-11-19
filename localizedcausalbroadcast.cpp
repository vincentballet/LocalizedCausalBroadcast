//
//  localizedcausalbroadcast.cpp
//  DA_PaymentSystem
//
//  Created by Vincent Ballet on 17/11/2018.
//  Copyright © 2018 Vincent Ballet. All rights reserved.
//

#include "localizedcausalbroadcast.h"
#include <iostream>
#include <cassert>
#include <cstring>
#include <cmath>

using std::cout;
using std::endl;

void LocalizedCausalBroadcast::onMessage(unsigned logical_source, const char* message, unsigned length)
{
    // must have at least 4 bytes for seq num
    assert(length >= 4);
    assert(logical_source <= senders.size() + 1);
   

    // process is affected by logical source
    if(this->loc.find(logical_source) != this->loc.end())
    {
        mtx_recv.lock();

        // obtaining the content
        string content(message + 4 + m, length - (4 + m));
        
        // obtaining the clock
        uint8_t* W[m];
        // TODO Not sure how to decypher this
        // memcpy(message + 4, W, m);
        
        // obtaining seq num
        unsigned seq_num = charsToInt32(message);
    
        // adding a message to the list in any case
        // will deliver it if it's correct, adding to front to be faster
        buffer[logical_source][seq_num] = content;
        
        // trying to deliver all messages
        tryDeliverAll(logical_source);
        
        mtx_recv.unlock();
    }
    // FIFO has done its job, we deliver 
    else {
        deliverToAll(logical_source, message, sizeof message / sizeof message[0]);
    }
    
}

bool LocalizedCausalBroadcast::tryDeliver(unsigned seq_num, unsigned source, std::string message)
{
    // TODO implement

    return false;
}

void LocalizedCausalBroadcast::tryDeliverAll(unsigned sender)
{
    // TODO implement

}

LocalizedCausalBroadcast::LocalizedCausalBroadcast(Broadcast *broadcast, set<unsigned> locality, int m, int n) : Broadcast(broadcast->this_process, broadcast->senders, broadcast->receivers)
{
    // init new vlock of size m (whatever the locality is)
    vclock = new uint8_t[m];
    
    this->m = m;
    // rank
    // TODO build a function for this if n is ill-defined
    this->rank = n - 1;
    
    // locality
    this->loc = locality;
    
    // sending sequence number is initially 1
    send_seq_num = 1;
    
    // allocating data for the buffer
    // n = |links| + 1
    // +1 for indexing from 1 instead of 0
    buffer = new map<unsigned, string>[senders.size() + 2];
    
    // saving broadcast object
    this->b = broadcast;
    
    // initializing broadcast
    b->addTarget(this);
}

LocalizedCausalBroadcast::~LocalizedCausalBroadcast()
{
    free(buffer);
}

void LocalizedCausalBroadcast::broadcast(const char* message, unsigned length, unsigned source)
{
    // buffer for sending
    char buffer[MAXLEN];
    
    mtx_send.lock();
    
    // incrementing sequence number
    unsigned seqnum = send_seq_num++;
    
    // updating the vclock
    vclock[this->rank] = seqnum;

    // copying vector clock
    memcpy(buffer + 4, vclock, min(m, MAXLEN - 4));

    mtx_send.unlock();
    
    // copying sequence number
    int32ToChars(seqnum, buffer);
    
    // copying payload
    memcpy(buffer + 4 + m, message, min(length, MAXLEN - (4 + m)));
    
    // broadcasting data
    b->broadcast(buffer, min(length + m + 4, MAXLEN), source);
}

bool LocalizedCausalBroadcast::compare_vclocks(uint8_t* W){
    for (int i = 0; i < m; i++) {
        if (!(vclock[i] <= W[i])) return false;
    }
    return true;
}
