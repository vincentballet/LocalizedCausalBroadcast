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
    // TODO implement
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

LocalizedCausalBroadcast::LocalizedCausalBroadcast(Broadcast *broadcast, map<unsigned, list<unsigned>> locality, int m) : Broadcast(broadcast->this_process, broadcast->senders, broadcast->receivers)
{
    // init new vlock of size m (whatever the locality is)
    vclock = new uint8_t[m];
    
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
    // TODO implement
}

bool LocalizedCausalBroadcast::compare_vclocks(uint8_t* W){
    int size = sizeof W / sizeof W[0];
    assert(size == sizeof vclock / sizeof vclock[0]);
    
    for (int i = 0; i < size; i++) {
        if (!(vclock[i] <= W[i])) return false;
    }
    
    return true;
}
