#include "receiver.h"

void Receiver::deliverToAll(unsigned source, char *message, unsigned length)
{
    vector<Target*>::iterator it;
    for(it = targets.begin(); it != targets.end(); it++)
    {
        Target* t = *it;
        t->onMessage(source, message, length);
    }
}

Receiver::Receiver(int this_process, Target *target)
{
    if(target)
        this->targets.push_back(target);
    this->this_process = this_process;
}

int Receiver::getThis()
{
    return this_process;
}

void Receiver::addTarget(Target *target)
{
    if(target)
        this->targets.push_back(target);
}
