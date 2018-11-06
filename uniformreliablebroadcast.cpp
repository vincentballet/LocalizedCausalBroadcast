#include "uniformreliablebroadcast.h"
#include "common.h"
#include <cstring>
#include <cassert>

// Protocol: first 4 bytes: physical source, then bytes

void UniformReliableBroadcast::onMessage(unsigned source, unsigned logical_source, const char* buffer, unsigned length)
{
    // string with the content
    string content(buffer, length);

    // pair (message, sender)
    pair<string, unsigned> content_source = make_pair(content, logical_source);

    // need to relay?
    bool need_broadcast = false;

    // critical section
    m.lock();

    // message already delivered, no need to do anything else
    if(delivered.find(content_source) != delivered.end())
    {
        m.unlock();
        return;
    }

    // this message is also an acknowledgement of itself
    if(ack.find(content_source) == ack.end())
    {
        need_broadcast = true;
        ack[content_source] = set<unsigned>();
    }

    // adding source as an acker
    ack[content_source].insert(source);

    // adding myself as an acker
    ack[content_source].insert(this_process);

    // can deliver => adding to the set
    if(canDeliver(content_source)) {
        ready_for_delivery.insert(content_source);
    }

#ifdef URB_DEBUG
    stringstream ss;
    ss << "urback " << charsToInt32(buffer + 4) << " " << source;
    memorylog->log(ss.str());
#endif

    // end of critical section
    m.unlock();

    // broadcasting if needed
    if(need_broadcast)
        b->broadcast(buffer, length, logical_source);

    // delivering all messages from the ready_for_delivery array
    tryDeliver();
}

void UniformReliableBroadcast::broadcast(const char* message, unsigned length, unsigned source)
{
    // beginning of critical section
    m.lock();

    // adding message to pending
    string content = string(message, length);

    // content + source
    pair<string, unsigned> content_source = make_pair(content, source);

    // if the message was sent previously, doing nothing now
    if(ack.find(content_source) != ack.end())
    {
        m.unlock();
        return;
    }

    // adding to the ACKed array
    ack[content_source] = set<unsigned>();

    // adding source and this process as ackers (if they are different)
    ack[content_source].insert(source);
    ack[content_source].insert(this_process);

    // end of critical section
    m.unlock();

    // broadcasting the message
    b->broadcast(message, length, source);
}

bool UniformReliableBroadcast::canDeliver(pair<string, unsigned> content_source)
{
    // otherwise must be acknowledged by > half of members (others + me)
    return 2 * ack[content_source].size() > (senders.size() + 1);
}

bool UniformReliableBroadcast::tryDeliver()
{
    // iterator for pending
    set<pair<string, unsigned> >::iterator it;

    // critical section
    m.lock();

    // subset of messages which can be delivered
    vector<pair<string, unsigned>> to_deliver;

    // loop over pending
    for(it = ready_for_delivery.begin(); it != ready_for_delivery.end(); it++)
    {
        // adding to the local buffer
        to_deliver.push_back(*it);

        // marking as delivered
        delivered.insert(*it);

        // no need to keep it in ACK
        ack.erase(*it);
    }

    // clearing the ready_for_delivery array
    ready_for_delivery.clear();

    // end of critical section
    m.unlock();

    // delivering everything that was in the buffer
    vector<pair<string, unsigned>>::iterator it1;
    for(it1 = to_deliver.begin(); it1 != to_deliver.end(); it1++)
    {
        string message = (*it1).first;
        unsigned source = (*it1).second;
#ifdef URB_DEBUG
        // log BEB deliver
        stringstream ss;
        ss << "urbd " << source << " " << charsToInt32(message.c_str() + 4);
        memorylog->log(ss.str());
#endif
        deliverToAll(source, message.c_str(), message.length());
    }
}

UniformReliableBroadcast::UniformReliableBroadcast(Broadcast *broadcast) : Broadcast(broadcast->this_process, broadcast->senders, broadcast->receivers)
{
    // saving the underlying broadcast
    this->b = broadcast;

    // adding this object as the target
    b->addTarget(this);
}
