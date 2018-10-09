#include "sender.h"
#include <string.h>
#include <iostream>

using std::cout;
using std::endl;

Sender::Sender(int destination)
{
    this->destination = destination;
}

void Sender::send(std::string data)
{
    send(data.c_str());
}

void Sender::send(char *data)
{
    send(data, strlen(data) + 1);
}

void Sender::send(char *data, int N)
{
    cout << "SENDING " << N << " bytes starting with" << data[0] << endl;
}

int Sender::getTarget()
{
    return destination;
}
