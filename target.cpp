#include "target.h"
#include <iostream>

using std::cout;
using std::endl;

Target::Target()
{

}

void Target::onMessage(unsigned source, char *buffer, unsigned length)
{
    cout << "onMessage(" << source << " " << (void*) buffer << " " << length << ")" << endl;
}
