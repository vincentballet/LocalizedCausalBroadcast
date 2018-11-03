#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "udpreceiver.h"
#include "common.h"
#include <iostream>

using namespace std;

// protocol: <first byte == source ID> <Data bytes...>

UDPReceiver::UDPReceiver(Membership *membership, int n, Target *target) : Receiver(n, target)
{
    fd = -1;
    struct sockaddr_in si_me;

    this->membership = membership;

    //create a UDP socket
    if ((fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        perror("Cannot open the socket");
        exit(-1);
    }

    // zero out the structure
    bzero(&si_me, sizeof(si_me));

    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(membership->getPort(n));
    si_me.sin_addr.s_addr = inet_addr(membership->getIP(n).c_str());

    //bind socket to port
    if(bind(fd, (struct sockaddr*) &si_me, sizeof(si_me)) == -1)
    {
        perror("Cannot bind");
        exit(-1);
    }

    pthread_create(&thread, nullptr, &UDPReceiver::receiveLoop, this);
}

void UDPReceiver::halt()
{
    close(fd);
}

UDPReceiver::~UDPReceiver()
{
    close(fd);
}

int UDPReceiver::receive(char* data, int maxlen)
{
    struct sockaddr_in si_other;
    socklen_t slen = sizeof(si_other);
    int recv_len;

    if((recv_len = recvfrom(fd, data, maxlen, 0, (struct sockaddr *) &si_other, &slen)) == -1)
    {
        //perror("Error in recvfrom");
        usleep(50000);
        return 0;
    };

    return recv_len;
}

void* UDPReceiver::receiveLoop(void *args)
{
    UDPReceiver* instance = (UDPReceiver*) args;

    // buffer for receiving
    char buffer[MAXLEN];

    while(true)
    {
        int len = instance->receive(buffer, MAXLEN);
        if(len == 0) continue;
        // <first byte sender 1> <0x01 1> <PF seq number 4 bytes> <logical sender 4> <fifo sequence number 4> <data 4>
        int source = buffer[0];

#ifdef UDP_DEBUG
        int type = buffer[1];
        int pk_seq = charsToInt32(buffer + 2);
        int log_sender = charsToInt32(buffer + 6);
        int fifo_seq = charsToInt32(buffer + 10);
        int payload = charsToInt32(buffer + 14);

        if(type == 0x01)
        {
            stringstream ss;
            ss << "Source " << source << " type " << type
               << " pkseq " << pk_seq << " log_sender " << log_sender
               << " fifo_seq " << fifo_seq << " payload " << payload;
            memorylog->log(ss.str());
        }
#endif

        if(source >= 0)
            instance->deliverToAll(buffer[0], buffer + 1, len - 1);
    }

    // no return
}
