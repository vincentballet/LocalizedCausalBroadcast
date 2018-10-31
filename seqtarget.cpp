#include <sstream>
#include "seqtarget.h"
#include <cassert>

SeqTarget::SeqTarget(int n, int maxSeq) : maxSeq(maxSeq)
{
    seqs.assign(n + 1, -1);
}

void SeqTarget::onMessage(unsigned logical_source, char *buffer, unsigned length)
{
    // checking the size
    assert(logical_source < seqs.size());

    // message payload
    int msg = charsToInt32(buffer);

    // updating the maximal message
    seqs[logical_source] = max(msg, seqs[logical_source]);

    // printing the message to the log
    if(length != 4) return;
    stringstream ss;
    ss << "d " << logical_source << " " << msg;
    memorylog->log(ss.str());
}

bool SeqTarget::isFull()
{
    bool full = true;
    for(int i = 1; i < seqs.size(); i++)
    {
        full &= (seqs[i] >= maxSeq + i * 1000);
    }
    return full;
}
