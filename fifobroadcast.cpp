#include "fifobroadcast.h"
#include <iostream>
#include <cassert>
#include <cstring>
#include <cmath>

using std::cout;
using std::endl;

void FIFOBroadcast::onMessage(char *buffer, unsigned length)
{
    // must have 8 bytes at least
    assert(length >= 8);

    // source is first 4 bytes
    unsigned source = charsToInt32(buffer);

    // continuing message parsing
    onMessage(source, buffer + 4, length - 4);
}

void FIFOBroadcast::onMessage(unsigned source, char *buffer, unsigned length)
{
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
    onMessage(msg);
}

bool FIFOBroadcast::tryDeliver(FIFOMessage m)
{
    // if current sequence number if 1 + old sequence number
    // from this sender, to ensure FRB5
    if(recv_seq_num[m.source] + 1 == m.seq_num)
    {
        // delivering
        deliver(m.buffer, m.length);

        // increasing seq num
        recv_seq_num[m.source] += 1;

        // successfully delivered
        return true;
    }

    // this message is too old/too new
    return false;
}

void FIFOBroadcast::onMessage(FIFOMessage m)
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
                // sending this data again on delivery to ensure RB4
                /// @todo Currently can have an issue that process crashes right here and then the message is delivered by this process but not any other
                broadcast((*it).buffer, (*it).length, (*it).source);

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

FIFOBroadcast::FIFOBroadcast(unsigned this_process_id, vector<PerfectLink *> links)
{
    // saving current id
    this->this_process_id = this_process_id;

    // saving perfect links
    this->links = links;

    // sending sequence number is initially 0
    send_seq_num = 0;

    // expecting to receive 0
    recv_seq_num.assign(links.size(), -1);
}

void FIFOBroadcast::broadcast(char *message, unsigned length)
{
    // sending a message from this process
    broadcast(message, length, this_process_id);
}

void FIFOBroadcast::broadcast(char *message, unsigned length, unsigned source)
{
    // for loop over links
    vector<PerfectLink*>::iterator it;

    // delivering the message locally?
    /// @todo How to ensure it's not delivered twice?
    /// Need to add content check?
    deliver(message, length);

    // buffer for sending
    char buffer[MAXLEN];

    // copying source
    int32ToChars(source, buffer);

    // copying sequence number
    int32ToChars(send_seq_num, buffer + 4);

    // copying payload
    memcpy(buffer + 8, message, min(length, MAXLEN - 8));

    // sending data to all perfect links
    for(it = links.begin(); it != links.end(); it++)
    {
        PerfectLink* link = (*it);

        /// @todo make it work
        //link->send(buffer, length + 8)
    }

    // incrementing sequence number
    send_seq_num++;
}


void FIFOBroadcast::deliver(char *message, unsigned len)
{
    // some dummy code for showing the delivered message
    cout << "Got message of " << len << "bytes; pointer address is " << message << endl;
}
