#include <sstream>
#include "seqtarget.h"
#include <cassert>
#include <iostream>

using std::cerr;
using std::endl;

SeqTarget::SeqTarget(unsigned n, unsigned maxSeq) : maxSeq(maxSeq), n(n)
{
    seqs = new unsigned[n + 1];

    // sanity check
    assert(seqs);

    for(unsigned i = 1; i <= n; i++)
        seqs[i] = 0;
}

void SeqTarget::onMessage(unsigned source, unsigned logical_source, const char* buffer, unsigned length)
{
    onMessage(logical_source, buffer, length);
}

void SeqTarget::onMessage(unsigned logical_source, const char* buffer, unsigned length)
{
    //fprintf(stderr, "Proc %d SeqTarget::onMessage(%u %d %d)\n", n, logical_source, charsToInt32(buffer), length);
    // checking the size
    assert(logical_source <= n);
    assert(length == 4);

    // message payload
    unsigned msg = charsToInt32(buffer);

    // updating the maximal message
    seqs[logical_source] = max(msg, seqs[logical_source]);

    // printing the message to the log
    stringstream ss;
    ss << "d " << logical_source << " " << msg;
    memorylog->log(ss.str());
}

bool SeqTarget::isFull()
{
    bool full = true;
    for(unsigned i = 1; i <= n; i++)
    {
        volatile unsigned seq = seqs[i];
        unsigned dest_seq = maxSeq;
#ifdef UNIQUE_MESSAGES
        dest_seq += i * 1000;
#endif
        full = full && (seq >= dest_seq);
    }
    return full;
}

string SeqTarget::describe()
{
    stringstream ss;
    for(unsigned i = 1; i <= n; i++)
        ss << seqs[i] << " ";
    return ss.str();
}
