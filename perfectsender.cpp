#include "perfectsender.h"
#include "common.h"
#include <string.h>

// format: 4 bytes seq number, message

PerfectSender::PerfectSender(std::string host, int port, UDPReceiver* r)
{
    this->r = r;
    this->s = new UDPSender(host, port);
    seqnum = 0;
}

void PerfectSender::send(char *data, int N)
{
    // allocating new memory
    char* data1 = (char*) malloc(N + 4);
    memcpy(data1 + 4, data, N);

    // adding the sequence number
    int32ToChars(seqnum++, data1);

    char buf[10000];

    while(true)
    {
        s->send(data1, N + 4);
        int len = r->receive(buf, 10000);
        if(len == 7 && memmem(buf, 4, data1, 4) && memmem(buf + 4, 3, "ACK", 3))
        {
            // ack received
            break;
        }
    }

    free(data1);
}
