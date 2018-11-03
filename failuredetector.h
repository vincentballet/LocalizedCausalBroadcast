/** @file failuredetector.h
 * @brief Failure detector
 * @author Volodin Sergei, EPFL
 * @date October 2018
 */

#ifndef FAILUREDETECTOR_H
#define FAILUREDETECTOR_H

#include "sender.h"
#include "receiver.h"
#include "failuremonitor.h"
#include <string>
#include <chrono>
#include <pthread.h>

using std::chrono::steady_clock;

/** @class This class implements failure detector via heartbeats */
class FailureDetector : public Target
{
private:
    /// @brief Failure Monitor to inform
    FailureMonitor* monitor;

    /// @brief Sender object
    Sender* s;

    /// @brief Timeout
    int timeout_ms;

    /// @brief Heartbeat message
    static string heartbeat_ping, heartbeat_pong;

    /// @brief Last reply from the other side
    volatile long last_reply;

    /// @brief Ping Thread
    pthread_t thread;

    /**
     * @brief pingLoop Runs the ping loop
     * @param arg FailureDetector instance
     * @return nullptr
     */
    static void* pingLoop(void* arg);

    /**
     * @brief first_byte First byte for PING
     */
    static const int first_byte_ping = 0xAA;

    /**
     * @brief first_byte First byte for PONG
     */
    static const int first_byte_pong = 0xAB;
public:
    /**
     * @brief FailureDetector Constructs a failure detector
     * @param s The sender
     * @param r The receiver
     * @param timeout_ms Timeout in milliseconds
     * @param monitor The object to inform about a failure
     */
    FailureDetector(Sender *s, Receiver *r, unsigned timeout_ms, FailureMonitor *monitor = nullptr);

    /// @brief Set monitor after creation
    void setMonitor(FailureMonitor* monitor);

    /// @brief Called on message from link
    void onMessage(unsigned source, const char* buffer, unsigned length);
};

#endif // FAILUREDETECTOR_H
