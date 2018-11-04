#include "broadcast.h"
#include <iostream>

using std::cout;
using std::endl;

void Broadcast::onMessage(unsigned source, const char* buffer, unsigned length)
{
    // do nothing
    cout << "Broadcast::onMessage(" << source << ", " << buffer[0] << ", " << length << ") not implemented!" << endl;
}

void Broadcast::broadcast(const char* message, unsigned length, unsigned source)
{
    // do nothing
    cout << "Broadcast::broadcast(" << source << ", " << message[0] << ", " << length << ") not implemented!" << endl;
}

Broadcast::Broadcast(unsigned this_process, vector<Sender *> senders, vector<Receiver *> receivers) :
    Receiver(this_process)
{
    this->senders = senders;
    this->receivers = receivers;
}

void Broadcast::broadcastPublic(const char* message, unsigned length)
{
    // send message from this process
    broadcast(message, length, this_process);
}

bool Broadcast::validSource(unsigned source)
{
    vector<Sender*>::iterator it;
    for(it = senders.begin(); it != senders.end(); it++)
    {
        if((*it)->getTarget() == source)
            return true;
    }
    return false;
}
