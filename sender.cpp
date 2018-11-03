#include "sender.h"
#include <string.h>
#include <iostream>

using std::cout;
using std::endl;

Sender::Sender(unsigned destination)
{
    this->destination = destination;
}

void Sender::send(std::string data)
{
    send(data.c_str(), data.length());
}

void Sender::send(const char* data)
{
    send(data, strlen(data) + 1);
}

void Sender::send(const char* data, unsigned N)
{
    cout << "SENDING " << N << " bytes starting with" << data[0] << ")" << endl;
}

unsigned Sender::getTarget()
{
    return destination;
}
