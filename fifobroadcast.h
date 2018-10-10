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
#include "perfectlink.h"
#include "common.h"

using std::vector;
using std::list;

/** @brief Maximal buffer size */
const int MAXLEN = 100;

/** @class Contains a single FIFO message (internal data structure) */
struct FIFOMessage {
    unsigned source;
    int seq_num;
    char buffer[MAXLEN];
    unsigned length;
};

/** @class This class implements FIFO broadcast
 * @todo Connect PerfectLink and onMessage()
 * @todo Make methods thread-safe
 * @todo Fix send() call
 * @todo Test the class */
class FIFOBroadcast : public Receiver, public Target
{
private:
    /// @brief A vector of Perfect Links (destinations)
    vector<PerfectLink*> links;

    /// @brief Receiving sequence numbers
    vector<int> recv_seq_num;

    /// @brief The buffer for not yet delivered messages
    list<FIFOMessage> buffer;

    /// @brief current sending sequence number
    int send_seq_num;

    /// @brief React on a message with parsed source
    void onMessage(unsigned source, char* buffer, unsigned length);

    /// @brief React on a parsed message
    void onMessage1(FIFOMessage m);

    /**
     * @brief tryDeliver Try delivering a message
     * @param m The message to deliver
     * @return True iff message was successfully delivered
     */
    bool tryDeliver(FIFOMessage m);

    /** @brief Broadcast a message with source other than this process */
    void broadcast(char* message, unsigned length, unsigned source);

    /** @brief Identificator of current process */
    unsigned this_process_id;
public:
    /**
     * @brief FIFOBroadcast initialization
     * @param this_process_id ID of the current process
     * @param links Vector of PerfectLink pointers connected to members     
     */
    FIFOBroadcast(unsigned this_process_id, vector<PerfectLink*> links);

    virtual ~FIFOBroadcast() {}

    /**
     * @brief Send a message using FIFO Broadcast algorithm
     * @param message A string to be sent
     * @param length The length of the message in bytes
     */
    void broadcast(char* message, unsigned length);
};

#endif // FIFOBROADCAST_H
