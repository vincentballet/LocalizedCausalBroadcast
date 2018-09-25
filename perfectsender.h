#ifndef PERFECTSENDER_H
#define PERFECTSENDER_H

#include "sender.h"
#include "udpsender.h"
#include "udpreceiver.h"

class PerfectSender : public Sender
{
private:
    UDPSender* s;
    UDPReceiver* r;
public:
    PerfectSender(string host, int port, UDPReceiver* r);
    ~PerfectSender() {}

    using Sender::send;
    /**
     * @brief send array of bytes
     * @param data array of bytes
     * @param N length of the array
     */
    virtual void send(char* data, int N);
};

#endif // PERFECTSENDER_H
