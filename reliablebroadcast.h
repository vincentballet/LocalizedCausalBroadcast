/** @file reliablebroadcast.h
 * @brief This file implements the reliable broadcast using Best Effort broadcast
 * @date October 2018
 * @author Volodin Sergei, 2018
 */

#ifndef RELIABLEBROADCAST_H
#define RELIABLEBROADCAST_H

#include "broadcast.h"
#include "failuredetector.h"
#include "besteffortbroadcast.h"
#include <string>
#include <set>
#include <map>
#include <list>

using std::set;
using std::map;
using std::list;

class FIFOBroadcast;

/** This class implements reliable broadcast algorithm using a best effort broadcast instance */
class ReliableBroadcast : public Broadcast, public FailureMonitor
{
    friend class FIFOBroadcast;
private:
    /// @brief from array (Algo 3.2)
    map<int, set<string> > from;

    /// @brief Correct processes (Algo 3.2)
    list<int> correct;

    /// @brief BestEffort broadcast instance
    BestEffortBroadcast* beb_broadcast;

    /// @brief Failure detectors for links
    vector<FailureDetector*> detectors;

    /// @brief React on a message with parsed source
    virtual void onMessage(unsigned source, char* buffer, unsigned length);

    /** @brief Broadcast a message with source other than this process */
    virtual void broadcast(char* message, unsigned length, unsigned source);

    /** @brief Called by failure detector */
    virtual void onFailure(int process);

    /** @brief Is the process correct? */
    bool isCorrect(int process);
public:
    /**
     * @brief Broadcast initialization
     * @param this_process_id ID of the current process
     * @param links Vector of PerfectLink pointers connected to members
     * @param timeout_ms Timeout in ms for failure detector
     */
    ReliableBroadcast(unsigned this_process, vector<PerfectLink*> links, int timeout_ms);

    virtual ~ReliableBroadcast() {}
};

#endif // RELIABLEBROADCAST_H
