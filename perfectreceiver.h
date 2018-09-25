#ifndef PERFECTRECEIVER_H
#define PERFECTRECEIVER_H

#include <string>
#include "udpreceiver.h"

using std::string;

class PerfectReceiver : public Receiver
{
private:
    UDPReceiver* r;
    int port;
    string host;
public:
    PerfectReceiver(UDPReceiver *r, string host, int port);

    virtual int receive(char* data, int N);
};

#endif // PERFECTRECEIVER_H
