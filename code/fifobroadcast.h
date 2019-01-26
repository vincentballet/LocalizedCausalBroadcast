/** @file fifobroadcast.h
 * @author Volodin Sergei, EPFL
 * @date October 2018
 * @brief This file is a header for the FIFO broadcast class
 *
 * Internal UDP message format:
 * <PerfectLink sequence> <Sender 4 bytes> <SeqNum 4 bytes> <Data bytes>
 */

#ifndef FIFOBROADCAST_H
#define FIFOBROADCAST_H

#include <vector>
#include <list>
#include "membership.h"
#include "udpreceiver.h"
#include "broadcast.h"
#include "common.h"
#include <mutex>

using std::vector;
using std::list;
using std::mutex;

/** @class This class implements FIFO broadcast */
class FIFOBroadcast : public Broadcast
{
private:
    /// @brief Mutex for the object
    mutex mtx_send, mtx_recv;

    /// @brief Reliable broadcast instance
    Broadcast* b;

    /// @brief Receiving sequence numbers
    map<unsigned, unsigned> recv_seq_num;

    /// @brief The buffer for not yet delivered messages per sender
    /// format: source -> (seq_num -> data)
    map<unsigned, string>* buffer;

    /// @brief current sending sequence number
    unsigned send_seq_num;

    /// @brief React on a message with parsed source
    virtual void onMessage(unsigned logical_source, const char* buffer, unsigned length);

    /// @brief React on a parsed message
    void tryDeliverAll(unsigned sender);

    /**
     * @brief tryDeliver Try delivering a message
     * @param m The message to deliver
     * @return True iff message was successfully delivered
     */
    bool tryDeliver(unsigned seq_num, unsigned source, string message);

    /** @brief Broadcast a message with source other than this process */
    void broadcast(const char* message, unsigned length, unsigned source);
public:
    /**
     * @brief FIFOBroadcast initialization
     * @param this_process_id ID of the current process
     * @param timeout_ms Timeout for failure detector
     */
    FIFOBroadcast(Broadcast* broadcast);

    virtual ~FIFOBroadcast();
};

#endif // FIFOBROADCAST_H
