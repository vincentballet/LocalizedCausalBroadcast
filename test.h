//
//  test.h
//  DA_PaymentSystem
//
//  Created by Vincent Ballet on 20/10/2018.
//  Copyright © 2018 Vincent Ballet. All rights reserved.
//

#ifndef test_h
#define test_h

#include <stdio.h>
#include "perfectlink.h"
#include "udpsender.h"
#include "udpreceiver.h"

void testPerfectLink(unsigned n, unsigned m, vector<PerfectLink*> links);
void testUDP(unsigned n, vector<UDPSender*> senders, UDPReceiver* receiver);
void testLOG();

#endif /* test_h */
