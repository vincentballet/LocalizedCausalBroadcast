/** @file broadcast.h
 * @brief This file describes the interface for broadcast classes
 * @date October 2018
 * @author Sergei Volodin, EPFL
 */

#ifndef BROADCAST_H
#define BROADCAST_H

#include "receiver.h"
#include "target.h"
#include "perfectlink.h"

class FIFOBroadcast;
class ReliableBroadcast;
class UniformReliableBroadcast;

/** @class Broadcast interface used in the project */
class Broadcast : public Receiver, public Target
{
    friend class FIFOBroadcast;
    friend class ReliableBroadcast;
    friend class UniformReliableBroadcast;
protected:
    /// @brief A vector of Perfect Links (destinations)
    vector<PerfectLink*> links;

    /// @brief React on a message with parsed source
    virtual void onMessage(unsigned source, char* buffer, unsigned length);

    /** @brief Broadcast a message with source other than this process */
    virtual void broadcast(char* message, unsigned length, unsigned source);
public:
    /**
     * @brief Broadcast initialization
     * @param this_process_id ID of the current process
     * @param links Vector of PerfectLink pointers connected to members
     */
    Broadcast(unsigned this_process, vector<PerfectLink*> links);

    virtual ~Broadcast() {}

    /**
     * @brief Send a message using Broadcast algorithm
     * @param message A string to be sent
     * @param length The length of the message in bytes
     */
    void broadcastPublic(char* message, unsigned length);

    /**
     * @brief validSource Check if a source is valid
     * @param source The source to check
     * @return True if valid
     */
    bool validSource(int source);
public:
    Broadcast();
};

#endif // BROADCAST_H
