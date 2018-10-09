#include "target.h"
#include <iostream>

using std::cerr;
using std::endl;

Target::Target()
{

}

void Target::onMessage(unsigned source, char *buffer, unsigned length)
{
    cerr << "onMessage(" << source << " " << (void*) buffer << " " << length << ")" << endl;
}
