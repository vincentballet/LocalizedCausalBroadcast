//
//  localizedcausalbroadcast.cpp
//  DA_PaymentSystem
//
//  Created by Vincent Ballet on 17/11/2018.
//  Copyright © 2018 Vincent Ballet. All rights reserved.
//

#include <iostream>
#include <cassert>
#include <cstring>
#include <cmath>
#include <iomanip>
#include "localizedcausalbroadcast.h"

using std::cout;
using std::endl;


void LocalizedCausalBroadcast::onMessage(unsigned logical_source, const char* message, unsigned length)
{
    // must have at least 4 bytes for seq num
    assert(length >= n_process * 4);
    assert(logical_source <= n_process);

    // obtaining the content
    string content(message + (n_process * 4), length - (n_process * 4));
    
    // obtaining the clock
    uint32_t* W = new uint32_t[n_process];
    memcpy(W, message, (n_process * 9));

    // sanity check
    assert(W);
    
#ifdef LCB_DEBUG
    {
        stringstream ss;
        ss << "lcbrecv " << logical_source << " " << charsToInt32(content.c_str()) << " | [" << W[0] << "," << W[1] << ","<< W[2] << ","<< W[3] << ","<< W[4] << "]";
        memorylog->log(ss.str());
    }
#endif

    // lock for the messages buffer & V_recv
    mtx_recv_clock.lock();
    
    // representation of all the received messages (source, payload, vectorclock)
    tuple<string, uint32_t*> t(content, W);

    // adding message to the buffer
    // -1 because indexed at 0
    buffer.at(logical_source-1)->insert(t);

    // trying to deliver all messages
    tryDeliverAll();
    
    
    // Looking at buffer of proces 1 and only when messages comes from process 3
//    if (this->rank == 0){
//        set<pair<string, uint32_t*>, cmpStruct>::iterator it;
//        cout<< "\n=== DEBUG PRINTING === " << endl;
//        for(int p = 0; p < n_process; p++){
//            cout << "== Process " << p + 1 <<endl;
//            for(it = buffer.at(p)->begin(); it != buffer.at(p)->end(); ++it )
//            {
//                uint32_t* W = it->second;
//                cout << "DEBUG S" << p << " : [" << std::setfill('0') << std::setw(2) << W[0] << "," <<  std::setfill('0') << std::setw(2) <<W[1] << ","<< std::setfill('0') << std::setw(2) << W[2] << ","<<  std::setfill('0') << std::setw(2) <<W[3] << ","<<  std::setfill('0') << std::setw(2) <<W[4] << "]" << endl;
//            }
//        }
//    }
    
    mtx_recv_clock.unlock();


#ifdef LCB_DEBUG
    {
        stringstream ss;
        ss << "lcbrecv " << logical_source << " " << charsToInt32(content.c_str()) << " END";
        memorylog->log(ss.str());
    }
#endif
}

void LocalizedCausalBroadcast::tryDeliverAll()
{
    // for going over buffer
    set<pair<string, uint32_t*>, cmpStruct>::iterator it;
    
    // indicates whether or not need one more pass over the buffer
    bool needMoreLoops = true;

    // doing multiple passes over the buffer
    // because otherwise can have order 2 1, then need two passes to deliver 1 2
    // ordering will fix this issue
    /// @todo Sort data over W and return if can't deliver anything else
    while(needMoreLoops)
    {
        needMoreLoops = false;
        // loop over all the processes
        
        
        for(int p = 0; p < n_process; p++){

            // loop over all messages from process p
            for(it = buffer.at(p)->begin(); it != buffer.at(p)->end(); )
            {
                // logical sender id
                unsigned sender = p + 1;
                // payload
                string content = it->first;
                // V_send at sender at time of broadcasting the received message
                uint32_t* W = it->second;

                // CRB delivering a message if W <= V_recv
                if(compare_vclocks(W, V_recv))
                {
#ifdef LCB_DEBUG
                    stringstream ss;
                    ss << "lcbd " << sender << " " << charsToInt32(content.c_str()) << " | [" << W[0] << "," << W[1] << ","<< W[2] << ","<< W[3] << ","<< W[4] << "] Vrecv=[" << V_recv[0] << "," << V_recv[1] << ","<< V_recv[2] << ","<< V_recv[3] << ","<< V_recv[4] << "]";
                    memorylog->log(ss.str());
#endif
                    // if have a message delivered, potentially can have more
                    needMoreLoops = true;

                    // increments nbr of received messages
                    V_recv[Membership::getRank(sender)] += 1;

                    // source of message is affecting receiver in terms of LCB
                    if(loc.find(sender) != loc.end()){
                        // mtx for send_seq_num & V_send clock
                        mtx_send_clock.lock();

                        // increments the current nbr of dependencies
                        V_send[Membership::getRank(sender)] += 1;

                        // unlocking the V_send lock
                        mtx_send_clock.unlock();
                    }

                    // passing to target
                    deliverToAll(sender, content.c_str(), content.length());

                    // free memory
                    free(W);

                    // erase() returns the next element
                    it = buffer.at(p)->erase(it);

                }
                // otherwise just incrementing the counter (can't deliver now)
                else {
#ifdef LCB_DEBUG
                    stringstream ss;
                    ss << "lcb can't deliver " << sender << " " << charsToInt32(content.c_str()) << " | [" << W[0] << "," << W[1] << ","<< W[2] << ","<< W[3] << ","<< W[4] << "] Vrecv=[" << V_recv[0] << "," << V_recv[1] << ","<< V_recv[2] << ","<< V_recv[3] << ","<< V_recv[4] << "]";
                    memorylog->log(ss.str());
#endif
                    // If we cannot deliver the message from the ordered set, then we cannot deliver any, so break.
                    break;
                }
            }
        }
    }
}

LocalizedCausalBroadcast::LocalizedCausalBroadcast(Broadcast *broadcast, set<unsigned> locality, unsigned rank) : Broadcast(broadcast->this_process, broadcast->senders, broadcast->receivers)
{
    // number of processes
    n_process = (unsigned int)this->senders.size() + 1;
    
    // init new vlock of size m (whatever the locality is)
    V_send = new uint32_t[n_process];
    V_recv = new uint32_t[n_process];
    
    // allocating memory for the buffer
    for(int p = 0; p < n_process; p++){
        buffer.push_back(new set<pair<string, uint32_t*>, cmpStruct>(cmpStruct(p)));
    }
    
    // sanity check
    assert(V_send);
    assert(V_recv);

    // zeroing vector clocks
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
    for(int p = 0; p < n_process; p++){
        free(buffer.at(p));
    }
}

void LocalizedCausalBroadcast::broadcast(const char* message, unsigned length, unsigned source)
{
    // buffer for sending
    char buffer[MAXLEN];

    // send vector clock copy
    uint32_t* W = new uint32_t[n_process];
    
    // sanity check
    assert(W);

    // mtx for send_seq_num & V_send clock
    mtx_send_clock.lock();

    // updating the sending vclock
    memcpy(W, V_send, n_process * 4);

    // WARNING: must use CURRENT send_seq_num
    W[this->rank] = send_seq_num;

    // incrementing sequence number
    send_seq_num++;

    // unlocking
    mtx_send_clock.unlock();
    
    // copying vector clock to the buffer
    memcpy(buffer, W, min(n_process * 4, MAXLEN));

    // copying payload
    memcpy(buffer + (n_process * 4), message, min(length, MAXLEN - (n_process * 4)));
    
#ifdef LCB_DEBUG
    stringstream ss;
    ss << "lcbb " << this_process << " " << charsToInt32(message) << " | [" << W[0] << "," << W[1] << ","<< W[2] << ","<< W[3] << ","<< W[4] << "]";
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

