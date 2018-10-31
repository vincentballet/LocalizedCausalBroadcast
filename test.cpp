//
//  test.cpp
//  DA_PaymentSystem
//
//  Created by Vincent Ballet on 20/10/2018.
//  Copyright © 2018 Vincent Ballet. All rights reserved.
//

#include "test.h"
#include <string>
#include <iostream>
#include <cassert>
#include <unistd.h>
#include "common.h"
#define private public
#include "perfectlink.h"
#include "udpsender.h"
#include "udpreceiver.h"
#include "membership.h"

using std::cout;
using std::endl;

//test if msgs is empty at the beginning
//and if sending empty buffers or nullptr
//is ignored
//void testPerfectLinkSend1(PerfectLink* l)
//{
//    assert(l->msgs.size() == 0);
//
//    char buffer[MAXLEN];
//    l->send(buffer, 0);
//    l->send(nullptr, 0);
//
//    assert(l->msgs.size() == 0 && l->seqnum == 0);
//
//}

//Sending some messages through a perfect link
//to be tested in log file afterwards
void runPerfectLink(PerfectLink* l, int nbr){
    
    char buffer[16];
    for (int i = 0; i < nbr; i++){
        l->send(buffer, 16);
    }
}


void testPerfectLink(int n, vector<PerfectLink*> links)
{
        
    int nMessages = 23;

    vector<PerfectLink*>::iterator it;
    for(it = links.begin(); it != links.end(); it++)
    {
        runPerfectLink(*it, nMessages);
    }

}
