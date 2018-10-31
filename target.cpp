#include "target.h"
#include <iostream>
#include <typeinfo>

using std::cout;
using std::endl;

Target::Target()
{

}

void Target::onMessage(unsigned source, unsigned logical_source, char *buffer, unsigned length)
{
    cout << "onMessage(" << source << " " << logical_source << " "
         << (void*) buffer << " " << length << ")" << " object: " << (typeid(*this).name()) << endl;
}

void Target::onMessage(unsigned source, char *buffer, unsigned length)
{
    cout << "onMessage(" << source << " " << (void*) buffer << " " << length << ")"
         << " object: " << (typeid(*this).name()) << endl;
}
