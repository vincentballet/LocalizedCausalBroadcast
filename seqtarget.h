/** @file seqtarget.h
 * @brief Print senders and sequence numbers of messages
 * @author Volodin Sergei, 2018
 * @date October 2018
 */

#ifndef SEQTARGET_H
#define SEQTARGET_H

#include "target.h"
#include "common.h"

/** Print sequence numbers and senders */
class SeqTarget : public Target
{
public:
    SeqTarget();

    /**
     * @brief onMessage Print message
     * @param source
     * @param buffer
     * @param length
     */
    virtual void onMessage(unsigned source, char* buffer, unsigned length);
};

#endif // SEQTARGET_H
