#include "fifobroadcast.h"
#include <iostream>
#include <cassert>
#include <cstring>
#include <cmath>

using std::cout;
using std::endl;

void FIFOBroadcast::onMessage(unsigned logical_source, const char* message, unsigned length)
{
    // must have at least 4 bytes for seq num
    assert(length >= 4);
    assert(logical_source <= senders.size() + 1);

    // obtaining the content
    string content(message + 4, length - 4);

    // obtaining seq num
    unsigned seq_num = charsToInt32(message);

    mtx_recv.lock();

    // no need to deliver if received a message which was already seen
    if(seq_num > recv_seq_num[logical_source])
    {
        // adding a message to the list in any case
        // will deliver it if it's correct, adding to front to be faster
        buffer[logical_source][seq_num] = content;

        // trying to deliver all messages
        tryDeliverAll(logical_source);
    }

    mtx_recv.unlock();
}

bool FIFOBroadcast::tryDeliver(unsigned seq_num, unsigned source, std::string message)
{
    // if current sequence number if 1 + old sequence number
    // from this sender, to ensure FRB5
    if(recv_seq_num[source] + 1 == seq_num)
    {
        // delivering
        deliverToAll(source, message.c_str(), message.length());

        // increasing seq num
        recv_seq_num[source] += 1;

        // successfully delivered
        return true;
    }

    // this message is too old/too new
    return false;
}

void FIFOBroadcast::tryDeliverAll(unsigned sender)
{
    // for going over buffer
    map<unsigned, string>::iterator it;

    // loop over buffer
    for(it = buffer[sender].begin(); it != buffer[sender].end(); )
    {
        unsigned seq_num = (*it).first;

        // some sequence number is greater than we need
        // since they only grow, there cannot be a delivery at this point
        // because stl::map is ordering the keys in ASCENDING order
        // http://www.cplusplus.com/reference/map/map/begin/
        if(recv_seq_num[sender] + 1 < seq_num)
        {
            break;
        }

        string content = (*it).second;

        // if can deliver, erasing current element
        if(tryDeliver(seq_num, sender, content))
            // erase() returns the next element
            it = buffer[sender].erase(it);

        // otherwise just incrementing the counter
        else it++;
    }
}

FIFOBroadcast::FIFOBroadcast(Broadcast *broadcast) : Broadcast(broadcast->this_process, broadcast->senders, broadcast->receivers)
{
    // sending sequence number is initially 1
    send_seq_num = 1;

    // allocating data for the buffer
    // n = |links| + 1
    // +1 for indexing from 1 instead of 0
    buffer = new map<unsigned, string>[senders.size() + 2];

    // sanity check
    assert(buffer);

    // expecting to receive 1
    vector<Sender*>::iterator it;
    for(it = senders.begin(); it != senders.end(); it++)
        recv_seq_num[(*it)->getTarget()] = 0;
    recv_seq_num[this_process] = 0;

    // saving broadcast object
    this->b = broadcast;

    // initializing broadcast
    b->addTarget(this);
}

FIFOBroadcast::~FIFOBroadcast()
{
    free(buffer);
}

void FIFOBroadcast::broadcast(const char* message, unsigned length, unsigned source)
{
    mtx_send.lock();

    // incrementing sequence number
    unsigned seqnum = send_seq_num++;

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
