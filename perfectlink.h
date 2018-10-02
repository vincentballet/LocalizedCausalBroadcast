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

class PerfectLink
{
private:
    std::map<int, char *> msgs;
    UDPSender* s;
    UDPReceiver* r;
    int m;
    int seqnum;
    int window;
    void craftAndStoreMsg();
    void waitForAcksOrTimeout();
public:
    PerfectLink(UDPSender *s, UDPReceiver *r, int m);
    void send();
};

#endif /* perfectlink_h */
