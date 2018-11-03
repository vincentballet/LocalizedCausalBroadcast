#include "receiver.h"

void Receiver::deliverToAll(unsigned source, unsigned logical_source, const char* message, unsigned length)
{
    vector<Target*>::iterator it;
    for(it = targets.begin(); it != targets.end(); it++)
    {
        Target* t = *it;
        t->onMessage(source, logical_source, message, length);
    }
}

void Receiver::deliverToAll(unsigned source, const char* message, unsigned length)
{
    vector<Target*>::iterator it;
    for(it = targets.begin(); it != targets.end(); it++)
    {
        Target* t = *it;
        t->onMessage(source, message, length);
    }
}

Receiver::Receiver(unsigned this_process, Target *target)
{
    if(target)
        this->targets.push_back(target);
    this->this_process = this_process;
}

unsigned Receiver::getThis()
{
    return this_process;
}

void Receiver::addTarget(Target *target)
{
    if(target)
        this->targets.push_back(target);
}
