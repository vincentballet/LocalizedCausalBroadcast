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
#include "besteffortbroadcast.h"
#include "fifobroadcast.h"
#include "localizedcausalbroadcast.h"
#include "uniformreliablebroadcast.h"
#include "seqtarget.h"
#include "test.h"
#include "pthread.h"
#include "threadedsender.h"
#include <sys/types.h>
#include <unistd.h>

using std::string;
using std::cout;
using std::endl;
using std::stringstream;

// wait for SIGUSR?
bool do_wait = true;

// number of messages to send
unsigned m = 10;

// got a command to send data already?
volatile bool sigusr_received = false;

// listening on our port
UDPReceiver *global_receiver;

// array of senders
vector<UDPSender*> *global_senders;

/**
 * @brief Handle the SIGUSR2 signal
 * @param signal_num must be SIGUSR2
 */
void onSignalUsr2(int signal_num)
{
    if(signal_num != SIGUSR2) return;
	
    // Start broadcasting messages
    sigusr_received = true;
}

/**
 * @brief Write current state and exit on application crash (SIGINT/SIGTERM)
 */
bool wasHalted = false;
void writeOutputAndHalt()
{
    if(wasHalted) return;
    wasHalted = true;
    cout << "Stopping networking..." << endl;
    // stop networking
    global_receiver->halt();
    vector<UDPSender*>::iterator it;
    for(it = global_senders->begin(); it != global_senders->end(); it++){
        (*it)->halt();
    }
    
    cout << "Writing output..." << endl;

    // The output is already in the file
    memorylog->dump();
    memorylog->close();

    exit(0);
}

/**
 * @brief Handle the SIGINT signal
 * @param signal_num must be SIGINT
 */
void onSignalInt(int signal_num)
{
    if(signal_num != SIGINT) return;
    cout << "SIGINT received" << endl;
    cout << "writeOutputAndHalt" << endl;
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

/// @brief Signal handler thread
// See man pthread_sigmask
static void *sig_thread(void *arg)
{
    // obtaining set of signals
    sigset_t *set = (sigset_t*) arg;
    int s, sig;

    for (;;) {
        // waiting for a signal...
        s = sigwait(set, &sig);

        // reacting on signal
        printf("Signal handling thread got signal %d\n", sig);

        // if signal is not for this function, it should ignore it
        onSignalUsr2(sig);
        onSignalInt(sig);
        onSignalTerm(sig);
    }
}

/**
 * @brief main() function for the project
 * @param argc Number of arguments
 * @param argv Array of parameters
 */
int main(int argc, const char** argv)
{
    // map signals to their handlers

    // temp variable
    int s;

    // set of signals
    sigset_t set;

    // thread which handles signals
    pthread_t signal_thread;

    // set := []
    sigemptyset(&set);

    // set += [USR2, INT, TERM]
    sigaddset(&set, SIGUSR2);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGTERM);

    // all threads don't care about signals from the set
    s = pthread_sigmask(SIG_BLOCK, &set, NULL);

    // apart from the newly created signal thread (see below)

    // argument processing
    argc--;

    // not enough arguments, show usage
    if(argc < 2)
    {
        cout << "Usage: " << argv[0] << " n membership [extra params...]" << endl;
        return 1;
    }
    
    // obtaining n and membership file
    unsigned n = (unsigned) atoi(argv[1]);

    // creating in-memory log
    memorylog = new InMemoryLog(n, "da_proc_" + string(argv[1]) + ".out");

#ifdef DEBUG_FILES
    // writing my PID
    ofstream("da_proc_" + string(argv[1]) + ".pid", std::ios::out) << getpid() << endl;
#endif

    // copying membership filename
    string membership = argv[2];
    
    // extra params (expected to be the # of messages)
    if (argc > 2){ m = atoi(argv[3]); }
    
    std::cout << "DEBUG | Process id: " << ::getpid() << " (parent: " << ::getppid() << ")" << std::endl;
    std::cout << "INFO  | Running process " << n << std::endl;

    // parsing membership file
    Membership members(membership);
    
    // printing membership data
    members.print();

    // list of processes
    vector<unsigned> processes = members.getProcesses();

    // checking if process is valid
    assert(members.validProcess(n));

    std::cout << "INFO  | Waiting for SIGUSR2 signal" << std::endl;

    // listening on our port
    UDPReceiver r(&members, n);

    // array of senders
    vector<UDPSender*> senders;

    // array with perfect links
    vector<PerfectLink*> links;

    // senders for the broadcast
    vector<Sender*> br_senders;

    // receivers for the broadcast
    vector<Receiver*> br_receivers;

    // saving global sender/receiver
    global_receiver = &r;
    global_senders = &senders;

    // creating links and senders
    vector<unsigned>::iterator it;
    for(it = processes.begin(); it != processes.end(); it++)
    {
        // not sending to myself
        if((*it) != n)
        {
            UDPSender* sender = new UDPSender(&members, *it, n);
            PerfectLink* link = new PerfectLink(sender, &r);
            senders.push_back(sender);
            links.push_back(link);

            // creating objects for broadcast
            br_receivers.push_back(link);
            br_senders.push_back(new ThreadedSender(link));
        }
    }

#ifdef DEBUG_TEST
    // if last argument is test, run tests
    string test = argv[argc];
    if (test.compare("test") == 0)
    {
        testPerfectLink(n, m, links);
        sleep(3);
        writeOutputAndHalt();
        return 0;    
    }
    else if (test.compare("testUDP") == 0)
    {
        testUDP(n, senders, &r);
        sleep(3);
        writeOutputAndHalt();
        return 0;
    }
    else if (test.compare("testLOG") == 0)
    {
        testLOG();
        sleep(3);
        writeOutputAndHalt();
        return 0;
    }
#endif
    
    // WARNING: MUST set up everything BEFORE waiting
    // for SIGUSR2. Otherwise can lose messages!

    // creating the best effort broadcast
    BestEffortBroadcast broadcast2(n, br_senders, br_receivers);

    // creating UR broadcast
    UniformReliableBroadcast broadcast1(&broadcast2);
    
#ifdef LOCALIZED_CAUSAL_BROADCAST
    // creating fifo broadcast
    FIFOBroadcast fifo(&broadcast1);
    // creating localized causal broadcast
    LocalizedCausalBroadcast broadcast(&fifo, members.getLocality(n), Membership::getRank(n));
#else
    // creating fifo broadcast
    FIFOBroadcast broadcast(&broadcast1);
#endif
    
    // printing sequence numbers
    SeqTarget t(processes.size(), m);
    broadcast.addTarget(&t);

    // starting listening to signals
    s = pthread_create(&signal_thread, NULL, &sig_thread, (void *) &set);

    // Waiting for sigusr2
    while(do_wait && sigusr_received == false)
    {
        usleep(10000);
    }

    std::cout << "INFO  | Sending data" << std::endl;

    // broadcasting messages
    char buf[4];
    for(unsigned i = 0; i < m; i++)
    {
        stringstream ss;
        unsigned payload = i + 1;
#ifdef UNIQUE_MESSAGES
            payload += 1000 * n;
#endif
        int32ToChars(payload, buf);
        ss << "b " << payload;
        memorylog->log(ss.str());
        broadcast.broadcastPublic(buf, 4);
    }

    // Waiting to be killed
    while(true)
    {
        usleep(10000);

#ifdef DEBUG_FILES
        if(t.isFull())
        {
            fprintf(stderr, "Process %d received all messages\n", n);
            stringstream ss, ss_fn;

            // writing a file
            ss_fn << "da_proc_" << n << ".recvall";
            ofstream f(ss_fn.str(), std::ios::out);
            f << "done";
            f.close();

            while(true)
            {
                usleep(100000);
            }
        }
#endif
    }

    // no return here
}
