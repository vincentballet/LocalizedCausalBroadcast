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
    for (unsigned i = 0; i < nbr; i++){
        l->send(buffer, 16);
    }
}


void testPerfectLink(unsigned n, unsigned m, vector<PerfectLink*> links)
{
        
    int nMessages = m;

    vector<PerfectLink*>::iterator it;
    for(it = links.begin(); it != links.end(); it++)
    {
        runPerfectLink(*it, nMessages);
    }

}

void testUDP(unsigned n, vector<UDPSender *> senders, UDPReceiver *receiver)
{
    vector<UDPSender*>::iterator sender_;
    for(sender_ = senders.begin(); sender_ != senders.end(); sender_++)
    {
        UDPSender* sender = *sender_;
        char buf[6];
        buf[0] = 0x01;
        for(unsigned i = 0; i < 10000000; i++)
        {
            int32ToChars(i, buf + 1);
            sender->send(buf, 5);
        }
    }
}

void* logger_thread(void* arg)
{
    // current thread number
    int threadid = *(int*) arg;

    // logs a lot of messages to the log
    char buf[100];

    //int n_msg = 100000000;
    int n_msg = 10000;

    printf("Thread %d will log %d messages...\n", threadid, n_msg);

    // for 100 000 000 messages...
    for(int i = 0; i < n_msg; i++)
    {
      snprintf(buf, 99, "process %d message %d", threadid, i);
      memorylog->log(buf);
    }

    return(nullptr);
}

void testLOG()
{
    int n_threads = 50;
    pthread_t* threads = new pthread_t[n_threads];
    int* args = new int[n_threads];
    for(int i = 0; i < n_threads; i++)
    {
        args[i] = i;
        pthread_create(&threads[i], nullptr, logger_thread, &args[i]);
    }
    for(int i = 0; i < n_threads; i++)
        pthread_join(threads[i], nullptr);
    delete[] threads;
    delete[] args;
}
