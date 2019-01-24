/** @file udpreceiver.h
 * @brief Receive UDP packets
 * @author Volodin Sergei, EPFL
 * @date Sep 25, 2018
 */

#ifndef UDPRECEIVER_H
#define UDPRECEIVER_H

#include <string>
#include "receiver.h"
#include "membership.h"

using std::string;

/** @brief Receive UDP packets */
class UDPReceiver : public Receiver
{
private:
    /// @brief The socket file descriptor
    int fd;

    /// @brief Memberships
    Membership* membership;

    /// @brief Receive thread
    pthread_t thread;

    /**
     * @brief Receive a datagram (blocking call)
     * @param data Set to a valid memory region
     * @param maxlen Maximal length to receive
     * @return Number of bytes received
     */
    virtual unsigned receive(char* data, unsigned maxlen);

    /**
     * @brief receiveLoop Starts the receive thread
     * @param args The UDPReceiver* as void*
     * @return nullptr
     */
    static void *receiveLoop(void *args);
public:
    /**
     * @brief Create new receiver
     * @param target The target object to relay messages to
     * @param membership The membership mapping
     * @param n The parameters of current process
     */
    UDPReceiver(Membership* membership, unsigned n, Target* target = nullptr);

    /**
     * @brief halt Stop the receiver
     */
    void halt();

    ~UDPReceiver();
};

#endif // UDPRECEIVER_H
