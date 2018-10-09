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
#include "common.h"
#include <list>
#include "inmemorylog.h"
#include "byteprinter.h"

using std::string;
using std::cout;
using std::endl;

InMemoryLog* log;

int m = 10;
bool sigusr_received = false;

/**
 * @brief Handle the SIGUSR1 signal
 * @param signal_num must be SIGUSR1
 */
void onSignalUsr1(int signal_num)
{
    if(signal_num != SIGUSR1) return;
	
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
    log->dump();

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
    // other host to send data to
    int other = 0;

    // map signals to their handlers
    signal(SIGTERM, onSignalTerm);
    signal(SIGINT, onSignalInt);
    signal(SIGUSR1, onSignalUsr1);

    // argument processing
    argc--;

    // not enough arguments, show usage
    if(argc < 2)
    {
        cout << "Usage: " << argv[0] << " n membership [extra params...]" << endl;
        return 1;
    }

    // obtaining n and membership file
    int n = atoi(argv[1]);

    log = new InMemoryLog(string(argv[1]).append(".log"));

	string membership = argv[2];
	
	// extra params (expected to be the # of messages)
	if (argc > 2){ m = atoi(argv[3]); }

	// check the parameters
	assert(1 <= n && n <= 5 && m > 0);

	std::cout << "DEBUG | Process id: " << ::getpid() << " (parent: " << ::getppid() << ")" << std::endl;
	std::cout << "INFO  | Running process " << n << std::endl;

    log->log("Loading membership");

    // parsing membership file
	Membership members(membership);

    std::cout << "INFO | Waiting for SIGUSR1 signal" << std::endl;

    // listening on our port
    /// @todo Shouldn't we SEND to another process, not this one again (n)?
    UDPReceiver r(&members, n);

    log->log("Waiting for SIGUSR1");

    // Waiting for sigusr1
    while(sigusr_received == false)
    {
        usleep(10000);
    }

    // send to another host
    other = n == 1 ? 2 : 1;

    // send to myself
    //other = n;

    // initializing sender
    UDPSender s(&members, other, n);

    // Target for messages
    Target t;
    BytePrinter p;

    r.addTarget(&p);

    // initializing perfect link
    PerfectLink p0(&s, &r, &t);

    // setting up failure detector

    // initializing failure detector
    FailureMonitor monitor;
    FailureDetector detector(&s, &r, 1000, &monitor);

    log->log("Sending data");

    for(int i = 0; i < m; i++)
        // sending messages
        p0.send(nullptr, 0);

    // Waiting to be killed
    while(true)
    {
        usleep(10000);
    }

    // no return here
}
