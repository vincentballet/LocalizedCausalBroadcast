#include "perfectsender.h"

PerfectSender::PerfectSender(std::string host, int port, UDPReceiver* r)
{
    this->r = r;
    this->s = new UDPSender(host, port);
}

void PerfectSender::send(char *data, int N)
{

}
