/** @file seqtarget.h
 * @brief Print senders and sequence numbers of messages
 * @author Volodin Sergei, 2018
 * @date October 2018
 */

#ifndef SEQTARGET_H
#define SEQTARGET_H

#include <vector>
#include "target.h"
#include "common.h"

using std::vector;

/** Print sequence numbers and senders */
class SeqTarget : public Target
{
    /// @brief Maximal message
    int maxSeq;

    /// @brief Current sequence number
    volatile int* volatile seqs;

    /// @brief Number of processes
    int n;
public:
    /**
     * @brief SeqTarget Accepts numbers as messages
     * @param maxSeq Maximal message to expect
     */
    SeqTarget(int n, int maxSeq);

    /**
     * @brief onMessage Print message
     * @param source from 1..n
     * @param buffer
     * @param length
     */
    virtual void onMessage(unsigned source, char* buffer, unsigned length);

    /**
     * @brief isFull Checks if maximal sequence number was reached
     * for all of the senders
     * @return True if reached
     */
    bool isFull();

    /** @brief Get received messages vector */
    string describe();
};

#endif // SEQTARGET_H
