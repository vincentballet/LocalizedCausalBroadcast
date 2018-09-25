/** @file udpsender.h
 * @brief Send UDP packets
 * @author Sergei Volodin, EPFL
 * @date Sep 25, 2018
 */

#ifndef UDPSENDER_H
#define UDPSENDER_H

#include <netinet/in.h>
#include <string>
#include "sender.h"

using std::string;

/** @brief Send UDP packets */
class UDPSender : public Sender
{
private:
    int fd;
    sockaddr_in servaddr;
public:
    /**
     * @brief Initialize sender
     * @param host Destination host
     * @param port Destination port
     */
    UDPSender(string host, int port);

    virtual ~UDPSender();

    using Sender::send;
    /**
     * @brief send array of bytes
     * @param data array of bytes
     * @param N length of the array
     */
    virtual void send(char* data, int N);
};

#endif // UDPSENDER_H
