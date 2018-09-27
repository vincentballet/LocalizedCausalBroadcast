#ifndef PERFECTSENDER_H
#define PERFECTSENDER_H

#include "sender.h"
#include "udpsender.h"
#include "udpreceiver.h"
#include <map>
#include <list>

class PerfectSender : public Sender
{
private:
    std::list<UDPSender>* s;
    UDPReceiver* r;
	int seqnum;
public:
    PerfectSender(std::list<UDPSender>*, UDPReceiver* r);
	int nAcks;
	int getNAcks(){ return nAcks; }

    ~PerfectSender() {}


    using Sender::send;
    /**
     * @brief send array of bytes
     * @param N length of the array
     */
    virtual void send(int N);
	void setNAcks(int n) { nAcks = n; }
};

#endif // PERFECTSENDER_H
