#include "uniformreliablebroadcast.h"

void UniformReliableBroadcast::onMessage(unsigned source, char *buffer, unsigned length)
{
    // string with the content
    string content(buffer, length);

    // pair (message, sender)
    pair<string, int> pend = make_pair(content, source);

    // need to relay?
    bool need_broadcast = false;

    // critical section
    m.lock();

    // this message is also an acknowledgement of itself
    if(ack.find(content) == ack.end())
        ack[content] = set<int>();
    ack[content].insert(source);

    // if message is not pending, add it there and relay
    if(pending.find(pend) == pending.end())
    {
        pending.insert(pend);
        need_broadcast = true;
    }

    // end of critical section
    m.unlock();

    // broadcasting if needed
    if(need_broadcast)
        b->broadcast(buffer, length, source);

    // ack and pending changed, checking if can deliver something now
    tryDeliverAll();
}

void UniformReliableBroadcast::broadcast(char *message, unsigned length, unsigned source)
{
    // beginning of critical section
    m.lock();

    // adding message to pending
    pending.insert(make_pair(string(message, length), source));

    // end of critical section
    m.unlock();

    // trying to deliver messages (in case N = 1)
    tryDeliverAll();

    // broadcasting the message
    b->broadcast(message, length, source);
}

bool UniformReliableBroadcast::canDeliver(std::string msg)
{
    // if message was never acknowledged, can't deliver it
    if(ack.find(msg) == ack.end()) return false;

    // otherwise must be acknowledged by > half of members (others + me)
    return 2 * ack[msg].size() > (links.size() + 1);
}

bool UniformReliableBroadcast::tryDeliver()
{
    // message buffer
    string message;

    // source buffer
    int source;

    // are buffers valid?
    bool found = false;

    // iterator for pending
    set<pair<string, int> >::iterator it;

    // critical section
    m.lock();

    // loop over pending
    for(it = pending.begin(); it != pending.end(); it++)
    {
        // if not delivered and can deliver
        if(delivered.find(message) == delivered.end() && canDeliver(message))
        {
            // filling in the buffers
            message = (*it).first;
            source = (*it).second;

            // marking message as delivered
            delivered.insert(message);

            // message is not pending anymore
            pending.erase(it);

            // buffer is valid now
            found = true;

            // stopping the loop
            break;
        }
    }

    // end of critical section
    m.unlock();

    // if buffer is valid, delivering its content
    if(found)
        deliverToAll(source, (char*) message.c_str(), message.length());

    // return true if there was a message delivered (therefore one more loop might be required)
    return found;
}

void UniformReliableBroadcast::tryDeliverAll()
{
    // trying to deliver messages until there are no messages to deliver
    while(true)
        if(!tryDeliver()) return;
}

UniformReliableBroadcast::UniformReliableBroadcast(Broadcast *broadcast) : Broadcast(broadcast->this_process, broadcast->links)
{
    // saving the underlying broadcast
    this->b = broadcast;
}
