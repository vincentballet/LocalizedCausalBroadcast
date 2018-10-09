#include "receiver.h"

Receiver::Receiver(int this_process, Target *target)
{
    this->target = target;
    this->this_process = this_process;
}

int Receiver::getThis()
{
    return this_process;
}

void Receiver::setTarget(Target *target)
{
    this->target = target;
}
