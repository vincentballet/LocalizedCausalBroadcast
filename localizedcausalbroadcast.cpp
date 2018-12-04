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
   
    // obtaining the content
    string content(message + 4 + (n_process * 4), length - (4 + (n_process * 4)));

    cout << (this->rank + 1) << " RCV from " << logical_source << " | " << content << endl;

    // process is affected by logical source
    if(loc.find(logical_source) != loc.end())
    {
        mtx_rcv.lock();
        
        // obtaining the clock
        /// @todo: MUST use new() because each message has a different vector clock
        // otherwise ALL of them will point to the same place in memory
        // and thus will have the same content
        // MOREOVER, in C++, a local variable points NOWHERE after the function ends (which
        // can happen here because we might want to deliver a message later),
        // so the content would be garbage (and trying to write it will result in segfault)
        // uint8_t W[n_process];
        uint32_t* W = new uint32_t[n_process];
        
        // TODO Not sure how to decypher this
        memcpy(W, message + 4, (n_process * 4));
        
        // obtaining seq num
        unsigned seq_num = charsToInt32(message);

        pair<string, uint32_t* > p (content, W);
        buffer[logical_source][seq_num] = p;
        
        // trying to deliver all messages
        tryDeliverAll(logical_source);
        
        mtx_rcv.unlock();
    }
    // FIFO has done its job, we deliver 
    else {
        // debug
        cout << (this->rank + 1) << " FIFO deliver " << logical_source << " | " << content << endl;
        deliverToAll(logical_source, content.c_str(), content.length());
    }
    
}

void LocalizedCausalBroadcast::tryDeliverAll(unsigned sender)
{
    // for going over buffer
    map<unsigned, pair<string, uint32_t*>>::iterator it;
    uint32_t* V = new uint32_t[n_process];

    mtx_clock.lock();
    memcpy(V, vclock, n_process * 4);
    mtx_clock.unlock();
    
    // loop over buffer
    for(it = buffer[sender].begin(); it != buffer[sender].end(); )
    {
        pair<string, uint32_t*> p = (*it).second;
        string content = p.first;
        uint32_t* W = p.second;
        
        

        if(compare_vclocks(V, W)){
            //CRB Delivering a message
            mtx_clock.lock();
            vclock[Membership::getRank(sender)] += 1;
            mtx_clock.unlock();
            
            //Incrementing our local variable for the loop to continue without issue
            V[Membership::getRank(sender)] += 1;

            //debug
            cout << this->rank << " CRB deliver " << sender << " | " << content << endl;
            deliverToAll(sender, content.c_str(), content.length());
            // erase() returns the next element
            it = buffer[sender].erase(it);
        }
        
        // otherwise just incrementing the counter
        else it++;
    }
}

LocalizedCausalBroadcast::LocalizedCausalBroadcast(Broadcast *broadcast, set<unsigned> locality, unsigned rank) : Broadcast(broadcast->this_process, broadcast->senders, broadcast->receivers)
{
    
    n_process = (unsigned int)this->senders.size() + 1;
    
    // init new vlock of size m (whatever the locality is)
    vclock = new uint32_t[n_process];
    for (int i = 0 ; i < n_process; i++)
        vclock[i] = 0;
    
    // rank
    // TODO build a function for this if n is ill-defined
    this->rank = rank;
    
    // locality
    this->loc = locality;
    
    // sending sequence number is initially 0 for clock comparison to work
    send_seq_num = 0;
    
    // allocating data for the buffer
    // n = |links| + 1
    // +1 for indexing from 1 instead of 0
    buffer = new map<unsigned, pair<string, uint32_t*>>[senders.size() + 2];
    
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
    uint32_t* W = new uint32_t[n_process];
    
    mtx_snd.lock();
    // updating the sending vclock
    mtx_clock.lock();
    memcpy(W, vclock, n_process * 4);
    mtx_clock.unlock();

    // incrementing sequence number
    unsigned seqnum = send_seq_num;
    W[this->rank] = seqnum;
    send_seq_num++;
    mtx_snd.unlock();

    // copying sequence number
    /// @todo Why do we need the sequence number if it's already inside the vector clock?
    int32ToChars(seqnum, buffer);
    
    // copying vector clock
    memcpy(buffer + 4, W, min(n_process * 4, MAXLEN - 4));

    // copying payload
    memcpy(buffer + 4 + (n_process * 4), message, min(length, MAXLEN - (4 + (n_process * 4))));
    
    // debug
    cout << "CRB sending " << seqnum << " | ";
    prettyprint(W, n_process);

    // broadcasting data
    b->broadcast(buffer, min(length + (n_process * 4) + 4, MAXLEN), source);

    // freeing up the memory
    delete[] W;
}

bool LocalizedCausalBroadcast::compare_vclocks(uint32_t* V, uint32_t* W){
    for (int i = 0; i < n_process; i++) {
        if (!(W[i] <= V[i])) return false;
    }
    return true;
}

