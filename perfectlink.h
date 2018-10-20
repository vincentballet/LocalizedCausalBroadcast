/** @file perfectlink.h
 * @brief Perfect Link
 * @author Vincent Ballet
 * @date 02/10/2018
 * Copyright © 2018 Vincent Ballet. All rights reserved.
 */

#ifndef perfectlink_h
#define perfectlink_h

#include <stdio.h>
#include <list>
#include <pthread.h>
#include <mutex>
#include <map>

#include "sender.h"
#include "receiver.h"
#include "failuredetector.h"

using std::mutex;
using std::map;
using std::pair;

/** @class This class implements the Perfect Link */
class PerfectLink : public Sender, public Receiver, public Target
{
private:
    map<int, pair<int, char*> > msgs;

    /** @brief Object for sending data to other host */
    Sender* s;

    /** @brief Object for receiving data from the other host */
    Receiver* r;

    /// @brief Send Thread
    pthread_t thread;

    /**
     * @brief sendLoop Runs the sending loop
     * @param arg PerfectLink instance
     * @return nullptr
     */
    static void* sendLoop(void* arg);
    
    /** @brief Current sequence number? Receive or send? */
    int seqnum;

    /** @brief Add message to send loop */
    void craftAndStoreMsg(char *buffer, int length);

    /** @brief Wait for an ACK */
    void waitForAcksOrTimeout();

    /** @brief Timeout for 1 message in microseconds (1e-6 sec) */
    unsigned const TIMEOUT = 100000;

    /** @brief A mutex to be used by the PerfectLink class */
    mutex mtx;

    /** @brief Called on message */
    void onMessage(unsigned source, char* buffer, unsigned length);
public:
    /**
     * @brief Establish a perfect link
     * @param s A UDPSender targeted at a host
     * @param r A UDPReceiver obtaining data from the same host
     */
    PerfectLink(Sender *s, Receiver *r, Target *target = nullptr);

    /**
     * @brief getSender Get the sender in the perfect link
     * @return Sender* pointer
     */
    Sender* getSender();

    /**
     * @brief getReceiver Get the receiver in this perfect link
     * @return Receiver* pointer
     */
    Receiver* getReceiver();

    /** @brief Send data */
    void send(char *buffer, int length);
};

#endif /* perfectlink_h */
