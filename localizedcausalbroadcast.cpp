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
    string content(message + (n_process * 4), length - (n_process * 4));
    
    // obtaining the clock
    uint32_t* W = new uint32_t[n_process];
    
    memcpy(W, message, (n_process * 4));
   
    assert(W);
    
#ifdef LCB_DEBUG
        stringstream ss;
        ss << "lcback from " << logical_source << " | [" << W[0] << "," << W[1] << ","<< W[2] << ","<< W[3] << ","<< W[4] << "]";
        memorylog->log(ss.str());
#endif

    /// Lock for the messages buffer & V_rcve
    mtx.lock();
    
    tuple<unsigned, string, uint32_t*> t(logical_source, content, W);
    buffer.push_back(t);

    // trying to deliver all messages
    tryDeliverAll();
    
    mtx.unlock();
    
}

void LocalizedCausalBroadcast::tryDeliverAll()
{
    // for going over buffer
    list<tuple<unsigned, string, uint32_t*>>::iterator it;

    
   
    // loop over messages in buffer
    for(it = buffer.begin(); it != buffer.end(); )
    {
        unsigned sender = std::get<0>(*it);
        string content = std::get<1>(*it);
        uint32_t* W = std::get<2>(*it);
        
#ifdef LCB_DEBUG
        stringstream ss;
        ss << "lcback CRB from " << sender << " | [" << W[0] << "," << W[1] << ","<< W[2] << ","<< W[3] << ","<< W[4] << "]";
        memorylog->log(ss.str());
#endif
            
        if(compare_vclocks(W, V_recv)){
            
            V_recv[Membership::getRank(sender)] += 1;
            
            if(loc.find(sender) != loc.end()){
                //CRB Delivering a message
//                mtx_send_clock.lock();
                V_send[Membership::getRank(sender)] += 1;
//                mtx_send_clock.unlock();
        
            }
            
            //passing to target
            deliverToAll(sender, content.c_str(), content.length());
            
            //free memory
            free(W);
            
            // erase() returns the next element
            it = buffer.erase(it);
            
        }

        // otherwise just incrementing the counter
        else it++;
    }
}

LocalizedCausalBroadcast::LocalizedCausalBroadcast(Broadcast *broadcast, set<unsigned> locality, unsigned rank) : Broadcast(broadcast->this_process, broadcast->senders, broadcast->receivers)
{
    
    n_process = (unsigned int)this->senders.size() + 1;
    
    // init new vlock of size m (whatever the locality is)
    V_send = new uint32_t[n_process];
    V_recv = new uint32_t[n_process];

    // sanity check
    assert(V_send);
    assert(V_recv);

    for (int i = 0 ; i < n_process; i++){
        V_send[i] = 0;
        V_recv[i] = 0;
    }

    // rank
    // TODO build a function for this if n is ill-defined
    this->rank = rank;
    
    // locality
    this->loc = locality;
    
    // sending sequence number is initially 0 for clock comparison to work
    send_seq_num = 0;

    // saving broadcast object
    this->b = broadcast;
    
    // initializing broadcast
    b->addTarget(this);
}

LocalizedCausalBroadcast::~LocalizedCausalBroadcast()
{
    free(V_send);
    free(V_recv);
}

void LocalizedCausalBroadcast::broadcast(const char* message, unsigned length, unsigned source)
{
    // buffer for sending
    char buffer[MAXLEN];
    uint32_t* W = new uint32_t[n_process];
    
    // sanity check
    assert(W);

    //mtx for send_seq_num
    mtx.lock();
    // updating the sending vclock
//    mtx_send_clock.lock();
    memcpy(W, V_send, n_process * 4);
//    mtx_send_clock.unlock();

    // incrementing sequence number
    unsigned seqnum = send_seq_num;
    W[this->rank] = seqnum;
    send_seq_num++ ;
    
    mtx.unlock();
    
    // copying vector clock
    memcpy(buffer, W, min(n_process * 4, MAXLEN));

    // copying payload
    memcpy(buffer + (n_process * 4), message, min(length, MAXLEN - (n_process * 4)));
    
#ifdef LCB_DEBUG
    stringstream ss;
    ss << "lcbsend" << " | [" << W[0] << "," << W[1] << ","<< W[2] << ","<< W[3] << ","<< W[4] << "]";
    memorylog->log(ss.str());
#endif

    // broadcasting data
    b->broadcast(buffer, min(length + (n_process * 4), MAXLEN), source);

    // freeing up the memory
    // todo difference here between delete[] and free?
    delete[] W;
}

bool LocalizedCausalBroadcast::compare_vclocks(uint32_t* W, uint32_t* V){
    for (int i = 0; i < n_process; i++) {
        if (W[i] > V[i]) return false;
    }
    return true;
}

