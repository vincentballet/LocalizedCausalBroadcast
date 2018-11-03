#include "fifobroadcast.h"
#include <iostream>
#include <cassert>
#include <cstring>
#include <cmath>

using std::cout;
using std::endl;

void FIFOBroadcast::onMessage(unsigned logical_source, const char* message, unsigned length)
{
    // resulting parsed message
    FIFOMessage msg;

    // must have at least 4 bytes for seq num
    assert(length >= 4);

    // filling in length
    msg.length = length - 4;

    // copying data
    memcpy(msg.buffer, message + 4, min(MAXLEN, length - 4));

    // obtaining seq num
    msg.seq_num = charsToInt32(message);

    // obtaining source
    msg.source = logical_source;

    mtx_recv.lock();

    // adding a message to the list in any case
    // will deliver it if it's correct, adding to front to be faster
    buffer.push_front(msg);

    // trying to deliver all messages
    tryDeliverAll();

    mtx_recv.unlock();
}

bool FIFOBroadcast::tryDeliver(FIFOMessage m)
{
    // if current sequence number if 1 + old sequence number
    // from this sender, to ensure FRB5
    if(recv_seq_num[m.source] + 1 == m.seq_num)
    {
        // delivering
        deliverToAll(m.source, m.buffer, m.length);

        // increasing seq num
        recv_seq_num[m.source] += 1;

        // successfully delivered
        return true;
    }

    // this message is too old/too new
    return false;
}

void FIFOBroadcast::tryDeliverAll()
{
    // for going over buffer
    list<FIFOMessage>::iterator it;

    // loop over buffer
    for(it = buffer.begin(); it != buffer.end(); )
    {
        // if can deliver, erasing current element
        if(tryDeliver(*it))
            // erase() returns the next element
            it = buffer.erase(it);

        // otherwise just incrementing the counter
        else it++;
    }
}

FIFOBroadcast::FIFOBroadcast(Broadcast *broadcast) : Broadcast(broadcast->this_process, broadcast->links)
{
    // sending sequence number is initially 0
    send_seq_num = 0;

    // expecting to receive 0
    vector<PerfectLink*>::iterator it;
    for(it = links.begin(); it != links.end(); it++)
        recv_seq_num[(*it)->getTarget()] = -1;
    recv_seq_num[this_process] = -1;

    // saving broadcast object
    this->b = broadcast;

    // initializing broadcast
    b->addTarget(this);
}

void FIFOBroadcast::broadcast(const char* message, unsigned length, unsigned source)
{
    mtx_send.lock();

    // incrementing sequence number
    int seqnum = send_seq_num++;

    mtx_send.unlock();

    // buffer for sending
    char buffer[MAXLEN];

    // copying sequence number
    int32ToChars(seqnum, buffer);

    // copying payload
    memcpy(buffer + 4, message, min(length, MAXLEN - 4));

    // broadcasting data
    b->broadcast(buffer, min(length + 4, MAXLEN), source);
}
