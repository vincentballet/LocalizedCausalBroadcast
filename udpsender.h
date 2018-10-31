/** @file udpsender.h
 * @brief Send UDP packets
 * @author Sergei Volodin, EPFL
 * @date Sep 25, 2018
 */

#ifndef UDPSENDER_H
#define UDPSENDER_H

#include <netinet/in.h>
#include <string>
#include <mutex>
#include "sender.h"
#include "membership.h"
#include "common.h"

using std::mutex;
using std::string;

/** @brief Send UDP packets */
class UDPSender : public Sender
{
private:
    /// @brief Socket to destination
    int fd;

    /// @brief Destination address
    sockaddr_in servaddr;

    /// @brief IP-ID mapping
    Membership* membership;

    /// @brief This process ID
    int this_process;

    /// @brief Target process ID
    int target;

    /// @brief Mutex for testing
    mutex mtx;
public:
    /**
     * @brief Initialize sender
     * @param membership The membership mapping
     * @param n The destination process
     * @param this_process This process ID (sender ID)
     */
    UDPSender(Membership* membership, int n, int this_process);

    virtual ~UDPSender();

    using Sender::send;
    /**
     * @brief send array of bytes
     * @param data array of bytes
     * @param N length of the array
     */
    virtual void send(char* data, int N);

    /**
     * @brief halt Stop the sender
     */
    void halt();
};

#endif // UDPSENDER_H
