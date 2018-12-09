//
//  localizedcausalbroadcast.h
//  DA_PaymentSystem
//
//  Created by Vincent Ballet on 17/11/2018.
//  Copyright © 2018 Vincent Ballet. All rights reserved.
//

#ifndef LOCALIZEDCAUSALBROADCAST_H
#define LOCALIZEDCAUSALBROADCAST_H

#include <vector>
#include <list>
#include <set>
#include "membership.h"
#include "udpreceiver.h"
#include "broadcast.h"
#include "common.h"
#include "membership.h"
#include <mutex>
#include <utility>

using std::pair;
using std::vector;
using std::list;
using std::tuple;
using std::set;
using std::mutex;

/** @class This class implements localized causal broadcast */
class LocalizedCausalBroadcast : public Broadcast
{
private:
    /// @brief Mutex for the object
    mutex mtx_snd, mtx_rcv, mtx_clock;
    
    /// @brief Reliable broadcast instance
    Broadcast* b;
    
    /// @todo What is this?
    unsigned rank;
    
    /// @todo What is this?
    unsigned n_process;
    
    /// @brief vector clock
    /// What is this? Which variable in the algorithm 3.15 is it?
    uint32_t* vclock;
    
    /// @brief Processes dependencies
    set<unsigned> loc;
    
    /// @brief The buffer for not yet delivered messages per sender
    /// format: (sender, content, vectorclock)
    list<tuple<unsigned, string, uint32_t*>> buffer;
    
    /// @brief current sending sequence number
    unsigned send_seq_num;
    
    /// @brief React on a message with parsed source
    virtual void onMessage(unsigned logical_source, const char* buffer, unsigned length);
    
    /// @brief React on all parsed messages
    void tryDeliverAll();
    
    /**
     * @brief tryDeliver Try delivering a message
     * @param m The message to deliver
     * @return True iff message was successfully delivered
     */
    bool tryDeliver(unsigned seq_num, unsigned source, string message);
    
    /** @brief Broadcast a message with source other than this process */
    void broadcast(const char* message, unsigned length, unsigned source);
    
    /** @brief vector clocks comparator */
    bool compare_vclocks(uint32_t* V, uint32_t* W);

public:
    /**
     * @brief LocalizedCausalBroadcast initialization
     * @param this_process_id ID of the current process
     * @param timeout_ms Timeout for failure detector
     */
    LocalizedCausalBroadcast(Broadcast* broadcast, std::set<unsigned> locality, unsigned rank);
    
    virtual ~LocalizedCausalBroadcast();
};

#endif // LOCALIZEDCAUSALBROADCAST_H
