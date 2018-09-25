#include "perfectreceiver.h"
#include "udpsender.h"
#include <string.h>

PerfectReceiver::PerfectReceiver(UDPReceiver *r, string host, int port)
{
    this->r = r;
    this->host = host;
    this->port = port;
}

int PerfectReceiver::receive(char *data, int N)
{
    char* data1 = (char*) malloc(N + 4);
    char* data2 = (char*) malloc(7);
    int len = r->receive(data1, N + 4);
    memcpy(data2, data1, 4);
    memcpy(data2 + 4, "ACK", 3);
    UDPSender s(host, port);
    s.send(data2, 7);
    memcpy(data, data1 + 4, len - 4);
    return len - 4;
}
