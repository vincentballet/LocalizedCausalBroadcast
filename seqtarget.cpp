#include <sstream>
#include "seqtarget.h"
#include <cassert>
#include <iostream>

using std::cerr;
using std::endl;

SeqTarget::SeqTarget(int n, int maxSeq) : maxSeq(maxSeq), n(n)
{
    seqs = new int[n + 1];
    for(int i = 1; i <= n; i++)
        seqs[i] = 0;
}

void SeqTarget::onMessage(unsigned logical_source, char *buffer, unsigned length)
{
    fprintf(stderr, "Proc %d SeqTarget::onMessage(%u %d %d)\n", n, logical_source, charsToInt32(buffer), length);
    // checking the size
    assert(logical_source <= n);
    assert(length == 4);

    // message payload
    int msg = charsToInt32(buffer);

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
    for(int i = 1; i <= n; i++)
    {
        full = full && (seqs[i] >= (maxSeq + i * 1000));
        cerr << "isFull " << full << endl;
    }
    return full;
}

string SeqTarget::describe()
{
    stringstream ss;
    for(int i = 1; i <= n; i++)
        ss << seqs[i] << " ";
    return ss.str();
}
