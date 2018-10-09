#include "byteprinter.h"
#include <iostream>

using namespace std;

BytePrinter::BytePrinter()
{

}

void BytePrinter::onMessage(unsigned source, char *buffer, unsigned length)
{
    cerr << "UDP message from ID " << source << " Len " << length << " First byte " << (int) buffer[1] << endl;
}
