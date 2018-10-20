//
//  test.cpp
//  DA_PaymentSystem
//
//  Created by Vincent Ballet on 20/10/2018.
//  Copyright © 2018 Vincent Ballet. All rights reserved.
//
#define private public

#include "test.h"
#include "perfectlink.h"
#include "udpsender.h"
#include "udpreceiver.h"
#include "membership.h"
#include "common.h"
#include <string>
#include <iostream>

using std::cout;
using std::endl;

void testPerfectLinkSend1(PerfectLink* l)
{
    //empty list at the beggining
    assert(l->msgs.size() == 0);
    
    char buffer[MAXLEN];
    l->send(buffer, 0);
    l->send(nullptr, 0);

    assert(l->msgs.size() == 0 && l->seqnum == 0);

}

void testPerfectLinkSend2(PerfectLink* l){
    
    char buffer[MAXLEN];
    for (int i = 0; i < 20; i++){
        int32ToChars(i, buffer);
        l->send(buffer, sizeof(i));
//        cout << "SIZE OF MSGS : " << l->msgs.size() << endl;
    }
}


void testPerfectLink()
{
    int p1 = 1;
    int p2 = 1;
    Membership members("membership");
    UDPReceiver* r = new UDPReceiver(&members, p1);
    UDPSender* s = new UDPSender(&members, p2, p1);
    PerfectLink* l = new PerfectLink(s, r);

//    testPerfectLinkSend1(l);
    testPerfectLinkSend2(l);
}



