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

Broadcast::Broadcast(unsigned this_process, vector<PerfectLink *> links) : Receiver(this_process)
{
    // saving perfect links
    this->links = links;
}

void Broadcast::broadcastPublic(const char* message, unsigned length)
{
    // send message from this process
    broadcast(message, length, this_process);
}

bool Broadcast::validSource(unsigned source)
{
    vector<PerfectLink*>::iterator it;
    for(it = links.begin(); it != links.end(); it++)
    {
        if((*it)->getTarget() == source)
            return true;
    }
    return false;
}
