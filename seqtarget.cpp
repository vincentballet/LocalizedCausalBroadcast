#include <sstream>
#include "seqtarget.h"

using std::stringstream;

SeqTarget::SeqTarget()
{

}

void SeqTarget::onMessage(unsigned logical_source, char *buffer, unsigned length)
{
    if(length != 4) return;
    stringstream ss;
    ss << "d " << logical_source << " " << charsToInt32(buffer);
    memorylog->log(ss.str());
}
