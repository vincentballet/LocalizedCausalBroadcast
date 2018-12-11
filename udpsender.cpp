#include "udpsender.h"
#include "common.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <cassert>

using std::cout;
using std::endl;

UDPSender::UDPSender(Membership *membership, unsigned n, unsigned this_process) : Sender(n)
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

void UDPSender::send(const char* data, unsigned N)
{
#ifdef UDPSENDER_DELAY_MS
    mtx.lock();
#endif
    // sanity check
    assert(N + 4 < MAXLEN);

    char buf[MAXLEN];

    unsigned len1 = min(N, MAXLEN - 4);

    memcpy(buf + 4, data, len1);
    int32ToChars(this_process, buf);

#ifdef UDP_DEBUG
    int type = buf[4];
    unsigned pk_seq = charsToInt32(buf + 5);
    unsigned log_sender = charsToInt32(buf + 9);
    unsigned payload = charsToInt32(buf + 13 + 4 * membership->getProcesses().size());

    if(type == 0x01)
    {
        stringstream ss;
        ss << "Destination " << target << " source " << this_process << " type " << type
           << " pkseq " << pk_seq << " log_sender " << log_sender
           << " payload " << payload;
        memorylog->log(ss.str());
    }

    if(type == 0x01)
    {
        stringstream ss;
        ss << "> udps\t" << this_process << " " << target; // << " "  << charsToInt32(data + 5 + 8 + 3);
        memorylog->log(ss.str());
    }
#endif

    if(sendto(fd, buf, len1 + 4, 0, (sockaddr*) &servaddr, sizeof(servaddr)) < 0)
    {
        //perror("Cannot send message");
    }

#ifdef UDPSENDER_DELAY_MS
    #warning "Delaying the sender!"
    usleep(1000 * UDPSENDER_DELAY_MS);
    mtx.unlock();
#endif
}

void UDPSender::halt()
{
    close(fd);
}
