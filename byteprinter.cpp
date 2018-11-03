#include "byteprinter.h"
#include <iostream>

using namespace std;

BytePrinter::BytePrinter()
{

}

void BytePrinter::onMessage(unsigned source, const char* buffer, unsigned length)
{
    cerr << "Message from ID " << source << " len=" << length << " data=";
    for(unsigned i = 0; i < length; i++)
        cerr << buffer[i];
    cerr << endl;
}
