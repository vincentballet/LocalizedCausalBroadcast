/** @file udpreceiver.h
 * @brief Receive UDP packets
 * @author Volodin Sergei, EPFL
 * @date Sep 25, 2018
 */

#ifndef UDPRECEIVER_H
#define UDPRECEIVER_H

#include <string>
#include "receiver.h"

using std::string;

/** @brief Receive UDP packets */
class UDPReceiver : public Receiver
{
private:
    int fd;
public:
    /**
     * @brief Create new receiver
     * @param host Host to listen on
     * @param port Port to listen on
     */
    UDPReceiver(string host, int port);

    ~UDPReceiver();

    /**
     * @brief Receive a datagram (blocking call)
     * @param data Set to a valid memory region
     * @param maxlen Maximal length to receive
     * @return Number of bytes received
     */
    virtual int receive(char* data, int maxlen);
};

#endif // UDPRECEIVER_H
