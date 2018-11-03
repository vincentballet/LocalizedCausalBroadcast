/** @file uniformreliablebroadcast.h
 * @author Volodin Sergei
 * @date October 2018
 * @brief This class implements Uniform Reliable Broadcast
 * With an assumption that at least half of the processes is correct
 */

#ifndef UNIFORMRELIABLEBROADCAST_H
#define UNIFORMRELIABLEBROADCAST_H

#include <set>
#include <map>
#include <mutex>
#include "broadcast.h"

using std::map;
using std::string;
using std::pair;
using std::set;

/** @classs This class implements Uniform Reliable Broadcast
 * With an assumption that at least half of the processes is correct
 */
class UniformReliableBroadcast : public Broadcast, public FailureMonitor
{
private:
    /// @brief Delivered messages
    /// @see p83 of the Book (Introduction to Reliable and Secure Distributed Programming, Cachin, Guerraou, Rodrigues), 2ed
    set<string> delivered;

    /// @brief Received but not yet delivered
    set<pair<string, unsigned> > pending;

    /// @brief Acknowledged messages
    map<string, set<unsigned> > ack;

    /// @brief React on a message with parsed source
    virtual void onMessage(unsigned source, unsigned logical_source, const char* buffer, unsigned length);

    /** @brief Broadcast a message with source other than this process */
    virtual void broadcast(const char* message, unsigned length, unsigned source);

    /** @brief Called by failure detector */
    virtual void onFailure(unsigned process);

    /// @brief Underlying broadcast object
    Broadcast* b;

    /// @brief Can deliver a message?
    bool canDeliver(string msg);

    /// @brief Try deliver messages from pending list
    bool tryDeliver();

    /// @brief Mutex for syncronization
    mutex m;
public:
    /**
     * @brief Broadcast initialization
     * @param broadcast Underlying broadcast implementation
     */
    UniformReliableBroadcast(Broadcast* broadcast);
};

#endif // UNIFORMRELIABLEBROADCAST_H
