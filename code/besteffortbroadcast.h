/** @file besteffortbroadcast.h
 * @brief This file implements the Best Effort broadcast
 * @date October 2018
 * @author Volodin Sergei, EPFL
 */

#ifndef BESTEFFORTBROADCAST_H
#define BESTEFFORTBROADCAST_H

#include "broadcast.h"

class ReliableBroadcast;

/** @class Implements best effort broadcast */
class BestEffortBroadcast : public Broadcast
{
    friend class ReliableBroadcast;
protected:
    /// @brief React on a message with parsed source
    virtual void onMessage(unsigned source, const char* buffer, unsigned length);

    /** @brief Broadcast a message with source other than this process */
    virtual void broadcast(const char* message, unsigned length, unsigned source);
public:
    /**
     * @brief Broadcast initialization
     * @param this_process_id ID of the current process
     * @param senders Senders list
     * @param receivers Receivers list
     */
    BestEffortBroadcast(unsigned this_process, vector<Sender*> senders,
                        vector<Receiver*> receivers);

    /**
     * @brief isClean
     * @return True if no messages in the send queue
     */
    bool isClean();
};

#endif // BESTEFFORTBROADCAST_H
