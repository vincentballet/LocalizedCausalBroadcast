#include "reliablebroadcast.h"

void ReliableBroadcast::onMessage(unsigned source, char *buffer, unsigned length)
{
    // checking if source is valid
    if(from.find(source) == from.end())
    {
        return;
    }

    // creating message as a string
    string message(buffer, length);

    // if message already in from, doing nothing
    if(from[source].find(message) != from[source].end())
    {
        return;
    }

    // delivering locally
    deliverToAll(source, buffer, length);

    // adding message to from
    from[source].insert(message);

    if(!isCorrect(source))
    {
        beb_broadcast->broadcast(buffer, length, source);
    }
}

void ReliableBroadcast::broadcast(char *message, unsigned length, unsigned source)
{
    // just running broadcast
    beb_broadcast->broadcast(message, length, source);
}

void ReliableBroadcast::onFailure(int process)
{
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

    // sending messages from failed process
    set<string>::iterator it1;
    for(it1 = from[process].begin(); it1 != from[process].end(); it1++)
    {
        string s = (*it1);
        beb_broadcast->broadcast((char*) s.c_str(), s.length(), process);
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

ReliableBroadcast::ReliableBroadcast(unsigned this_process, vector<PerfectLink *> links, int timeout_ms) :
    Broadcast(this_process, links)
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
    beb_broadcast = new BestEffortBroadcast(this_process, links);
    beb_broadcast->addTarget(this);
}
