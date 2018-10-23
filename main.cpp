/** @file main.cpp
 * @brief Main file for the Distributed Computing project
 * @date Sep 25 2018
 * @author Volodin Sergei, EPFL
 */

#include <cassert>
#include <climits>
#include <iostream>
#include <unistd.h>
#include <string>
#include <stdlib.h>
#include <csignal>
#include "membership.h"
#include "udpreceiver.h"
#include "udpsender.h"
#include "perfectlink.h"
#include <time.h>
#include <sstream>
#include <list>
#include "common.h"
#include "byteprinter.h"
#include "besteffortbroadcast.h"
#include "fifobroadcast.h"
#include "seqtarget.h"
#include "test.h"

using std::string;
using std::cout;
using std::endl;
using std::stringstream;

// wait for SIGUSR?
bool do_wait = true;

int m = 10;
bool sigusr_received = false;

/**
 * @brief Handle the SIGUSR2 signal
 * @param signal_num must be SIGUSR2
 */
void onSignalUsr1(int signal_num)
{
    if(signal_num != SIGUSR2) return;
	
    // Start broadcasting messages
    sigusr_received = true;
}

/**
 * @brief Write current state and exit on application crash (SIGINT/SIGTERM)
 */
void writeOutputAndHalt()
{
    //reset signal handlers to default
    signal(SIGTERM, SIG_DFL);
    signal(SIGINT, SIG_DFL);

    // stop networking
	
    // writing output file
    memorylog->dump();

    exit(0);
}

/**
 * @brief Handle the SIGINT signal
 * @param signal_num must be SIGINT
 */
void onSignalInt(int signal_num)
{
    if(signal_num != SIGINT) return;
    writeOutputAndHalt();
}


/**
 * @brief Handle the SIGTERM signal
 * @param signal_num must be SIGTERM
 */
void onSignalTerm(int signal_num)
{
    if(signal_num != SIGTERM) return;
    writeOutputAndHalt();
}

/**
 * @brief main() function for the project
 * @param argc Number of arguments
 * @param argv Array of parameters
 */
int main(int argc, char** argv)
{
    // map signals to their handlers
    signal(SIGTERM, onSignalTerm);
    signal(SIGINT, onSignalInt);
    signal(SIGUSR2, onSignalUsr1);

    // argument processing
    argc--;

    // not enough arguments, show usage
    if(argc < 2)
    {
        cout << "Usage: " << argv[0] << " n membership [extra params...]" << endl;
        return 1;
    }

    // creating in-memory log
    memorylog = new InMemoryLog("da_proc_" + string(argv[1]) + ".out");
    
    // obtaining n and membership file
    int n = atoi(argv[1]);

    // copying membership filename
    string membership = argv[2];
    
    // extra params (expected to be the # of messages)
    if (argc > 2){ m = atoi(argv[3]); }
    
    std::cout << "DEBUG | Process id: " << ::getpid() << " (parent: " << ::getppid() << ")" << std::endl;
    std::cout << "INFO  | Running process " << n << std::endl;

    // parsing membership file
    Membership members(membership);

    // list of processes
    vector<int> processes = members.getProcesses();

    // checking if process is valid
    assert(members.validProcess(n));

    std::cout << "INFO  | Waiting for SIGUSR2 signal" << std::endl;

    // listening on our port
    UDPReceiver r(&members, n);

    // array of senders
    vector<UDPSender*> senders;

    // array with perfect links
    vector<PerfectLink*> links;

    // creating links and senders
    vector<int>::iterator it;
    for(it = processes.begin(); it != processes.end(); it++)
    {
        // not sending to myself
        if((*it) != n)
        {
            UDPSender* sender = new UDPSender(&members, *it, n);
            PerfectLink* link = new PerfectLink(sender, &r);
            senders.push_back(sender);
            links.push_back(link);
        }
    }

    
    // if last argument is test, run tests
    string test = argv[argc];
    if (test.compare("test") == 0){
        testPerfectLink(n, links);
        sleep(3);
        writeOutputAndHalt();
        return 0;
        
    }
    
    // Waiting for sigusr1
    while(do_wait && sigusr_received == false)
    {
        usleep(10000);
    }
    
    // creating broadcast object
    FIFOBroadcast broadcast(n, links, 1000);

    // printing sequence numbers
    SeqTarget t;
    broadcast.addTarget(&t);

    std::cout << "INFO  | Sending data" << std::endl;

    // broadcasting messages
    char buf[4];
    for(int i = 0; i < m; i++)
    {
        stringstream ss;
        int32ToChars(i, buf);
        ss << "b " << i;
        memorylog->log(ss.str());
        broadcast.broadcastPublic(buf, 4);
    }

    // Waiting to be killed
    while(true)
    {
        usleep(10000);
    }

    // no return here
}
