#include "perfectsender.h"
#include "common.h"
#include <string.h>
#include <list>
#include <iostream>

// format: 4 bytes seq number, message

PerfectSender::PerfectSender(std::list<UDPSender>* s, UDPReceiver* r)
{
	this->r = r;
	this->s = s;

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

	std::cout << "b " << seqnum << std::endl;
	
    while(true)
    {
		for (std::list<UDPSender>::iterator it = this->s->begin(); it != this->s->end(); ++it){
			it->send(data, N + 4);
			int len = r->receive(buf, 10000);
			if(len == 7 && memmem(buf, 4, data, 4) && memmem(buf + 4, 3, "ACK", 3))
			{
				nAcks++;
				break;
			}
		}
    }

    free(data);
}
