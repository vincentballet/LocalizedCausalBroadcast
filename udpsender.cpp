#include "udpsender.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>

UDPSender::UDPSender(std::string host, int port)
{
    fd = socket(AF_INET, SOCK_DGRAM, 0);

    if(fd < 0)
    {
        perror("Cannot open socket");
        exit(0);
    }

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(host.c_str());
    servaddr.sin_port = htons(port);
}

UDPSender::~UDPSender()
{
    if(fd >= 0) close(fd);
}

void UDPSender::send(char *data, int N)
{
    if(sendto(fd, data, N, 0, (sockaddr*) &servaddr, sizeof(servaddr)) < 0)
    {
        perror("Cannot send message");
        close(fd);
        exit(0);
    }
}
