//
//  perfectlink.h
//  DA_PaymentSystem
//
//  Created by Vincent Ballet on 02/10/2018.
//  Copyright © 2018 Vincent Ballet. All rights reserved.
//

#ifndef perfectlink_h
#define perfectlink_h
#include "udpsender.h"
#include "udpreceiver.h"
#include <stdio.h>
#include <list>
#include <map>

using std::string;
using std::map;

/** @class This class implements the Perfect Link */
class PerfectLink
{
private:
    map<int, char *> msgs;

    /** @brief Object for sending data to other host */
    UDPSender* s;

    /** @brief Object for receiving data from the other host */
    UDPReceiver* r;
    int m;

    /** @brief Current sequence number? Receive or send? */
    int seqnum;

    /** @brief ??? */
    int window;

    /** @brief ??? */
    void craftAndStoreMsg();

    /** @brief ??? */
    void waitForAcksOrTimeout();
public:
    /**
     * @brief Establish a perfect link
     * @param s A UDPSender targeted at a host
     * @param r A UDPReceiver obtaining data from the same host
     * @param m ???
     */
    PerfectLink(UDPSender *s, UDPReceiver *r, int m);

    /** @brief ??? */
    void send();

    /** @brief Get target process ID */
    int getTarget();

    /** @todo Add onMessage() method and use it in other classes
     * It's logical to start the receiving thread here and not anywhere else */
};

#endif /* perfectlink_h */
