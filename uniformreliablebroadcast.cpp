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
        ack[content_source] = set<unsigned>();
    ack[content_source].insert(source);

    // adding myself as an acker
    ack[content_source].insert(this_process);

#ifdef URB_DEBUG
    stringstream ss;
    ss << "urback " << charsToInt32(buffer + 4) << " " << source;
    memorylog->log(ss.str());
#endif

    // if message is not pending, add it there and relay
    if(pending.find(content_source) == pending.end())
    {
        pending.insert(content_source);
        need_broadcast = true;
    }

    // end of critical section
    m.unlock();

    // broadcasting if needed
    if(need_broadcast)
        b->broadcast(buffer, length, logical_source);

    // ack and pending changed, checking if can deliver something now
    tryDeliver();
}

void UniformReliableBroadcast::broadcast(const char* message, unsigned length, unsigned source)
{
    // beginning of critical section
    m.lock();

    // adding message to pending
    string content = string(message, length);
    pending.insert(make_pair(content, source));

    // end of critical section
    m.unlock();

    // broadcasting the message
    b->broadcast(message, length, source);
}

bool UniformReliableBroadcast::canDeliver(pair<string, unsigned> content_source)
{
    // if message was never acknowledged, can't deliver it
    if(ack.find(content_source) == ack.end()) return false;

    // otherwise must be acknowledged by > half of members (others + me)
    return 2 * ack[content_source].size() > (links.size() + 1);
}

void UniformReliableBroadcast::onFailure(unsigned process)
{
    // destroying the perfect link
    vector<PerfectLink*>::iterator pit;
    for(pit = links.begin(); pit != links.end(); pit++)
    {
        if((*pit)->getTarget() == process)
        {
            // halting the perfect link on failure
            (*pit)->halt();
        }
    }
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
    for(it = pending.begin(); it != pending.end();)
    {
        // filling in the buffers
        string message = (*it).first;
        unsigned source = (*it).second;

        // if not delivered and can deliver
        if(delivered.find(*it) == delivered.end() && canDeliver(*it))
        {
            // marking message as delivered
            delivered.insert(*it);

            // saving the message
            to_deliver.push_back(make_pair(message, source));

            // no need to keep it in ACK
            ack.erase(*it);

            // erasing message from pending
            it = pending.erase(it);
        }
        else it++;
    }

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

UniformReliableBroadcast::UniformReliableBroadcast(Broadcast *broadcast) : Broadcast(broadcast->this_process, broadcast->links)
{
    // saving the underlying broadcast
    this->b = broadcast;

    // adding this object as the target
    b->addTarget(this);

#ifdef NO_PONG_DEAD_MS
    // creating failure detectors
    vector<PerfectLink*>::iterator it;
    for(it = links.begin(); it != links.end(); it++)
    {
        PerfectLink* link = *it;

        // adding failure detector for a link
        FailureDetector* detector = new FailureDetector(link->getSender(), link->getReceiver(), NO_PONG_DEAD_MS, this);

        // saving the detector
        detectors.push_back(detector);
    }
#endif
}
