#include "perfectsender.h"
#include "common.h"
#include <string.h>

// format: 4 bytes seq number, message

PerfectSender::PerfectSender(std::string host, int port, UDPReceiver* r)
{
	this->r = r;
    this->s = new UDPSender(host, port);
    seqnum = 0;
	nAcks = 0;
}

void PerfectSender::send(int N)
{
	// allocating new memory
    char* data = (char*) malloc(N + 4);

    // adding the sequence number
    int32ToChars(seqnum++, data);

    char buf[10000];

    while(true)
    {
        s->send(data, N + 4);
        int len = r->receive(buf, 10000);
        if(len == 7 && memmem(buf, 4, data, 4) && memmem(buf + 4, 3, "ACK", 3))
        {
			nAcks++;
            break;
        }
    }

    free(data);
}
