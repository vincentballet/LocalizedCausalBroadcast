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
	int seqnum;
public:
    PerfectSender(string host, int port, UDPReceiver* r);
	int nAcks;
	int getNAcks(){ return nAcks; }

    ~PerfectSender() {}


    using Sender::send;
    /**
     * @brief send array of bytes
     * @param N length of the array
     */
    virtual void send(int N);
};

#endif // PERFECTSENDER_H
