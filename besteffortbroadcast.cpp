#include "besteffortbroadcast.h"
#include "common.h"
#include <cstring>

void BestEffortBroadcast::onMessage(unsigned source, char *buffer, unsigned length)
{
    // log BEB deliver
    stringstream ss;
    ss << "bebd " << source << " " << charsToInt32(buffer + 8);
    memorylog->log(ss.str());

    // just delivering the data using logical sender
    deliverToAll(charsToInt32(buffer), buffer + 4, length - 4);
}

void BestEffortBroadcast::broadcast(char *message, unsigned length, unsigned source)
{
    // log BEB broadcast
    stringstream ss;
    ss << "bebb " << charsToInt32(message + 4);
    memorylog->log(ss.str());

    // for loop over links
    vector<PerfectLink*>::iterator it;

    // delivering the message locally
    /// @todo How to ensure it's not delivered twice?
    /// Need to add content check?
    deliverToAll(source, message, length);

    // buffer for sending
    char buffer[MAXLEN];

    // copying source
    int32ToChars(source, buffer);

    // copying payload
    memcpy(buffer + 4, message, min(length, MAXLEN - 4));

    // sending data to all perfect links
    for(it = links.begin(); it != links.end(); it++)
    {
        (*it)->send(buffer, length + 4);
    }
}

BestEffortBroadcast::BestEffortBroadcast(unsigned this_process, vector<PerfectLink *> links) :
    Broadcast (this_process, links)
{
    vector<PerfectLink*>::iterator it;
    for(it = links.begin(); it != links.end(); it++)
        (*it)->addTarget(this);
}
