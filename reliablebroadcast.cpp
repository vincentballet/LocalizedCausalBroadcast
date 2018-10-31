#include "reliablebroadcast.h"
#include "common.h"

void ReliableBroadcast::onMessage(unsigned source, char *buffer, unsigned length)
{
    if(source == this_process)
    {
        // delivering locally
        deliverToAll(source, buffer, length);
        return;
    }

    mtx.lock();

    // checking if source is valid
    if(from.find(source) == from.end())
    {
        mtx.unlock();
        return;
    }

    // creating message as a string
    string message(buffer, length);

    // if message already in from, doing nothing
    if(from[source].find(message) != from[source].end())
    {
        mtx.unlock();
        return;
    }

    // delivering locally
    deliverToAll(source, buffer, length);

    // adding message to from
    from[source].insert(message);

    mtx.unlock();

    if(!isCorrect(source))
    {
        b->broadcast(buffer, length, source);
    }
}

void ReliableBroadcast::broadcast(char *message, unsigned length, unsigned source)
{
    // just running broadcast
    b->broadcast(message, length, source);
}

void ReliableBroadcast::onFailure(int process)
{
    mtx.lock();
    // removing process from correct list
    list<int>::iterator it;
    for(it = correct.begin(); it != correct.end(); it++)
    {
        if(*it == process)
        {
            correct.erase(it);
            break;
        }
    }

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

    // need to copy data INSIDE the critical section
    set<string> relay = set<string>(from[process].begin(), from[process].end());

    // but send data outside, otherwise deadlock will happen
    // since bebBroadcast will trigger bebDeliver which will call rbDeliver
    mtx.unlock();

    // sending messages from failed process
    set<string>::iterator it1;
    for(it1 = relay.begin(); it1 != relay.end(); it1++)
    {
        string s = (*it1);
        b->broadcast((char*) s.c_str(), s.length(), process);
    }
}

bool ReliableBroadcast::isCorrect(int process)
{
    // removing process from correct list
    list<int>::iterator it;
    for(it = correct.begin(); it != correct.end(); it++)
    {
        if(*it == process)
        {
            return true;
        }
    }
    return false;
}

ReliableBroadcast::ReliableBroadcast(Broadcast *broadcast, int timeout_ms) :
    Broadcast (broadcast->this_process, broadcast->links)
{
    // creating failure detectors
    vector<PerfectLink*>::iterator it;
    for(it = links.begin(); it != links.end(); it++)
    {
        PerfectLink* link = *it;

        // add an empty set as from messages
        from[link->getTarget()] = set<string>();

        // adding process as correct
        correct.push_back(link->getTarget());

        // adding failure detector for a link
        FailureDetector* detector = new FailureDetector(link->getSender(), link->getReceiver(), timeout_ms, this);
        detectors.push_back(detector);
    }

    // creating best effort broadcast instance
    b = broadcast;
    b->addTarget(this);
}
