#include "udpsender.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include "common.h"

using std::cout;
using std::endl;

UDPSender::UDPSender(Membership *membership, int n, int this_process) : Sender(n)
{
    this->membership = membership;
    this->this_process = this_process;
    this->target = n;

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    if(fd < 0)
    {
        perror("Cannot open socket");
        exit(0);
    }

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(membership->getIP(n).c_str());
    servaddr.sin_port = htons(membership->getPort(n));
}

UDPSender::~UDPSender()
{
    if(fd >= 0) close(fd);
}

void UDPSender::send(char *data, int N)
{
    char buf[MAXLEN];

    buf[0] = this_process;

    int len1 = min(N, MAXLEN - 1);

    memcpy(buf + 1, data, len1);

    int source = buf[0];
    int type = buf[1];
    int pk_seq = charsToInt32(buf + 2);
    int log_sender = charsToInt32(buf + 6);
    int fifo_seq = charsToInt32(buf + 10);
    int payload = charsToInt32(buf + 14);

    if(type == 0x01)
    cout << "Destination " << target << " source " << source << " type " << type
         << " pkseq " << pk_seq << " log_sender " << log_sender
         << " fifo_seq " << fifo_seq << " payload " << payload << endl;

    if(sendto(fd, buf, len1 + 1, 0, (sockaddr*) &servaddr, sizeof(servaddr)) < 0)
    {
        perror("Cannot send message");
        close(fd);
        exit(0);
    }
}
