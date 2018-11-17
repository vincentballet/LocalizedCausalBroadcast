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

LocalizedCausalBroadcast::LocalizedCausalBroadcast(Broadcast *broadcast, map<unsigned, list<unsigned>> locality) : Broadcast(broadcast->this_process, broadcast->senders, broadcast->receivers)
{
    // TODO implement
}

LocalizedCausalBroadcast::~LocalizedCausalBroadcast()
{
    free(buffer);
}

void LocalizedCausalBroadcast::broadcast(const char* message, unsigned length, unsigned source)
{
    // TODO implement
}
