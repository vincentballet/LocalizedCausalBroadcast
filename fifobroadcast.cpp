#include "fifobroadcast.h"
#include <iostream>
#include <cassert>
#include <cstring>
#include <cmath>

using std::cout;
using std::endl;

void FIFOBroadcast::onMessage(unsigned source, char *buffer, unsigned length)
{
    mtx.lock();
    // resulting parsed message
    FIFOMessage msg;

    // must have at least 4 bytes for seq num
    assert(length >= 4);

    // filling in length
    msg.length = length - 4;

    // copying data
    memcpy(msg.buffer, buffer + 4, min(MAXLEN, length - 4));

    // obtaining seq num
    msg.seq_num = charsToInt32(buffer);

    // obtaining source
    msg.source = source;

    // processing message further
    onMessage1(msg);
    mtx.unlock();
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

void FIFOBroadcast::onMessage1(FIFOMessage m)
{
    // source is an index of a perfect link in links vector
    assert(m.source < links.size());

    // message payload length must be > 0
    assert(m.length > 0);

    // adding a message to the list in any case
    // will deliver it if it's correct, adding to front to be faster
    buffer.push_front(m);

    // need one more loop over list of not yet delivered messages?
    bool needMoreLoops = true;

    // for going over buffer
    list<FIFOMessage>::iterator it;

    // loop until nothing to deliver
    while(needMoreLoops)
    {
        // by default, there is nothing to deliver until something was found
        needMoreLoops = false;

        // loop over buffer
        for(it = buffer.begin(); it != buffer.end(); it++)
        {
            // trying to deliver current message
            if(tryDeliver(*it)) // SUCCESS
            {
                // after delivering the message it's no longer required
                buffer.erase(it);

                // need more loops because there was a message delivered
                needMoreLoops = true;

                // cannot continue the loop since iterator is now invalid...
                /// @todo fix that (otherwise can have O(N^2) time)
                break;
            }
        }
    }
}

FIFOBroadcast::FIFOBroadcast(unsigned this_process_id, vector<PerfectLink *> links, int timeout_ms) : Broadcast(this_process_id, links)
{
    // sending sequence number is initially 0
    send_seq_num = 0;

    // expecting to receive 0
    recv_seq_num.assign(links.size(), -1);

    // creating rb broadcast
    rb_broadcast = new ReliableBroadcast(this_process_id, links, timeout_ms);
    rb_broadcast->addTarget(this);
}

void FIFOBroadcast::broadcast(char *message, unsigned length, unsigned source)
{
    mtx.lock();

    // incrementing sequence number
    int seqnum = send_seq_num++;

    mtx.unlock();

    // for loop over links
    vector<PerfectLink*>::iterator it;

    // buffer for sending
    char buffer[MAXLEN];

    // copying sequence number
    int32ToChars(seqnum, buffer);

    // copying payload
    memcpy(buffer + 4, message, min(length, MAXLEN - 4));

    rb_broadcast->broadcast(message, length, source);
}
