//
//  perfectlink.cpp
//  DA_PaymentSystem
//
//  Created by Vincent Ballet on 02/10/2018.
//  Copyright © 2018 Vincent Ballet. All rights reserved.
//

#include "perfectlink.h"
#include "common.h"
#include <pthread.h>
#include <iostream>
#include <mutex>
#include <unistd.h>

using namespace std;

unsigned const MSG_SIZE = 1000;
unsigned const BUF_SIZE = 1000;
unsigned const TIMEOUT = 1;
std::mutex mtx;

struct thread_data {
    UDPSender* s;
    UDPReceiver* r;
    std::map<int, char*>* msgs;
};

void* receive(void *argthreads) {
    struct thread_data *args = (struct thread_data *)argthreads;
    UDPSender* s = args->s;
    UDPReceiver* r = args->r;
    std::map<int, char*>* msgs = args->msgs;
    
    // buffer for receiving messages
    char buf[BUF_SIZE];

    std::cout << "* Receiving thread is running" << std::endl;
    while(true){
        int len = r->receive(buf, MSG_SIZE);
        
        // receiving an ACK from a sent message
        if(len == 7 && memmem(buf + 4, 3, "ACK", 3))
        {
            int seqnumack = charsToInt32(buf);
            std::cout << "** Received ACK " << seqnumack << std::endl;
            mtx.lock();
            msgs->erase(seqnumack);
            mtx.unlock();
           
        } // receiving content from another process => we send an ACK
        else {
            int tmp = charsToInt32(buf);
            std::cout << "** Received content " << tmp << std::endl;
            char* sdata = (char*) malloc(7);
            memcpy(sdata, buf, 4);
            memcpy(sdata + 4, "ACK", 3);
            std::cout << "** Sending ACK for " << tmp << std::endl;
            s->send(sdata, 7);
        }
        
    }
    pthread_exit(NULL);
}

PerfectLink::PerfectLink(UDPSender* s, UDPReceiver* r, int m)
{
    this->s = s;
    this->r = r;
    this->m = m;
    this->seqnum = 0;
    this->window = 10;
}

void PerfectLink::send()
{
    
    pthread_t threadid;
    struct thread_data threadargs;
    threadargs.s = this->s;
    threadargs.r = this->r;
    threadargs.msgs = &this->msgs; // note the pointer not to mess with multiple lists
    
    // Running the receiving side of a different thread. Passing threadargs as params
    // receive is not a class function
    if (pthread_create(&threadid, NULL, receive, (void *)&threadargs)) {
        cout << "Error: unable to create thread" << endl;
        exit(-1);
    }
    
    
    while(true){
        // end of loop condition
        if (this->seqnum == this->m){
            break;
        } else {
            // Craft a new message if list contains less than 10 messages to be sent / waiting for acks
            if (this->msgs.size() < this->window && this->seqnum < this->m){
                std::cout << "> Crafting message " << this->seqnum << std::endl;
                craftAndStoreMsg();
            }
            
            // Testing purpose
            sleep(1);
            
            if (this->msgs.size() > 0){
                // Note : map does not keep insertion order but .begin()
                //returns elements with smallest key
                mtx.lock();
                char* sdata = this->msgs.begin()->second;
                std::cout << "> Sending " << this->msgs.begin()->first << std::endl;
                mtx.unlock();

                this->s->send(sdata, MSG_SIZE + 4);
            }
            
        }
    }
}

void PerfectLink::craftAndStoreMsg(){
    // allocating new memory
    char* data = (char*) malloc(MSG_SIZE + 4);
    
    // adding the sequence number
    int32ToChars(this->seqnum, data);

    // adding the message to the list
    mtx.lock();
    this->msgs[this->seqnum] = data;
    mtx.unlock();

    this->seqnum++;
}
