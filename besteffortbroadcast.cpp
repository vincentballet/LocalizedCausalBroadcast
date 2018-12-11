#include "besteffortbroadcast.h"
#include "common.h"
#include <cstring>

void BestEffortBroadcast::onMessage(unsigned source, const char* buffer, unsigned length)
{
    // log BEB deliver
#ifdef BEB_DEBUG
    stringstream ss;
    ss << "bebd " << source << " " << charsToInt32(buffer) << " " << charsToInt32(buffer + 4 + 4 * (senders.size() + 1));
//    printf("BEBD\n");
//    for(int i = 0; i < length; i++)
//    {
//        printf("%d\t", i);
//    }
//    printf("\n");
//    for(int i = 0; i < length; i++)
//    {
//        printf("0x%x\t", (uint8_t) buffer[i]);
//    }
//    printf("\n");
    memorylog->log(ss.str());
#endif

    // just delivering the data using logical sender
    deliverToAll(source, charsToInt32(buffer), buffer + 4, length - 4);
}

void BestEffortBroadcast::broadcast(const char* message, unsigned length, unsigned source)
{
#ifdef BEB_DEBUG
    // log BEB broadcast
    stringstream ss;
    ss << "bebb " << charsToInt32(message + 4);
    memorylog->log(ss.str());
#endif

    // for loop over links
    vector<Sender*>::iterator it;

    // delivering the message locally
    /// @todo How to ensure it's not delivered twice?
    /// Need to add content check?
    deliverToAll(source, source, message, length);

    // buffer for sending
    char buffer[MAXLEN];

    // copying source
    int32ToChars(source, buffer);

    // copying payload
    memcpy(buffer + 4, message, min(length, MAXLEN - 4));

    // sending data to all perfect links
    for(it = senders.begin(); it != senders.end(); it++)
    {
        (*it)->send(buffer, length + 4);
    }
}

BestEffortBroadcast::BestEffortBroadcast(unsigned this_process, vector<Sender *> senders, vector<Receiver *> receivers) :
    Broadcast(this_process, senders, receivers)
{
    vector<Receiver*>::iterator it;
    for(it = receivers.begin(); it != receivers.end(); it++)
        (*it)->addTarget(this);
}

bool BestEffortBroadcast::isClean()
{
    return false;
}
