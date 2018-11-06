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
#include <set>

#include "sender.h"
#include "receiver.h"
#include "threadedreceiver.h"
#include <semaphore.h>

using std::mutex;
using std::map;
using std::set;
using std::tuple;
using std::string;

/** @class This class implements the Perfect Link */
class PerfectLink : public Sender, public ThreadedReceiver, public Target
{
private:
    /// @brief Map seq number -> message, timestamp
    map<unsigned, tuple<unsigned, char*, long> > msgs;

    /** @brief Object for sending data to other host */
    Sender* s;

    /** @brief Object for receiving data from the other host */
    Receiver* r;

    /// @brief Send Thread
    pthread_t send_thread, ack_thread;

    /// @brief Fill semaphore for the msgs buffer
    /// @see https://en.wikipedia.org/wiki/Producer%E2%80%93consumer_problem
    sem_t fill_sem;

    /// @brief Empty semaphore for the msgs buffer
    /// @see https://en.wikipedia.org/wiki/Producer%E2%80%93consumer_problem
    sem_t empty_sem;

    /**
     * @brief sendLoop Runs the sending loop
     * @param arg PerfectLink instance
     * @return nullptr
     */
    static void* sendLoop(void* arg);

    /**
     * @brief ackLoop Runs the ACK loop
     * @param arg PerfectLink instance
     * @return nullptr
     */
    static void* ackLoop(void* arg);
    
    /** @brief List of delivered messages (not to get no-duplication violated) */
    set<string> delivered;
    
    /** @brief Current sequence number? Receive or send? */
    unsigned seqnum;

    /** @brief Wait for an ACK */
    void waitForNewMessagesOrTimeout();

    /** @brief Timeout between retries for 1 message in milliseconds (1e-3 sec)
     * @see perfectlink_config.cpp */
    static unsigned const TIMEOUT_MSG;

    /** @brief Maximal number of messages in the send queue (w/o ack)
     * Or the WINDOW SIZE
     * @see perfectlink_config.cpp */
    static unsigned const MAX_IN_QUEUE;

    /** @brief A mutex to be used by the PerfectLink class */
    mutex mtx;

    /** @brief Called on message */
    void onMessage(unsigned source, const char* buffer, unsigned length);

    /** @brief Set to false to disable */
    bool running = true;

    /** @brief true if send buffer is empty */
    volatile bool clean;

    /** @brief Messages now in queue */
    volatile unsigned inqueue;
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
    void send(const char* buffer, unsigned length);

    /**
     * @brief halt Stop the link (e.g. if destination crashed)
     */
    void halt();

    /**
     * @brief isClean
     * @return True if no messages in send queue
     */
    bool isClean();
};

#endif /* perfectlink_h */
