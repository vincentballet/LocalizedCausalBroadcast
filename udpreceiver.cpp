#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "udpreceiver.h"

UDPReceiver::UDPReceiver(std::string host, int port)
{
    fd = -1;
    struct sockaddr_in si_me;

    //create a UDP socket
    if ((fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        perror("Cannot open the socket");
        exit(-1);
    }

    // zero out the structure
    bzero(&si_me, sizeof(si_me));

    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(port);
    si_me.sin_addr.s_addr = inet_addr(host.c_str());
	
    //bind socket to port
    if(bind(fd, (struct sockaddr*) &si_me, sizeof(si_me)) == -1)
    {
        perror("Cannot bind");
        exit(-1);
    }
}

UDPReceiver::~UDPReceiver()
{
    close(fd);
}

int UDPReceiver::receive(char *data, int maxlen)
{
    struct sockaddr_in si_other;
    socklen_t slen = sizeof(si_other);
    int recv_len;

    if((recv_len = recvfrom(fd, data, maxlen, 0, (struct sockaddr *) &si_other, &slen)) == -1)
    {
        perror("Error in recvfrom");
        exit(-1);
    }

    return recv_len;
}
