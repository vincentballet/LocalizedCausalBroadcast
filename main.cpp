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
#include "common.h"


using std::string;
using std::cout;
using std::endl;

int m = 10;

/**
 * @brief Handle the SIGUSR1 signal
 * @param signal_num must be SIGUSR1
 */
void onSignalUsr1(int signal_num)
{
    if(signal_num != SIGUSR1) return;
	
    // Start broadcasting messages
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
	string membership = argv[2];
	
	// extra params (expected to be the # of messages)
	if (argc > 2){ m = atoi(argv[3]); }

	// check the parameters
	assert(1 <= n && n <= 5 && m > 0);
	
	
	std::cout << "DEBUG | Process id: " << ::getpid() << " (parent: " << ::getppid() << ")" << std::endl;
	std::cout << "INFO  | Running process " << n << std::endl;

    // parsing membership file
	Membership members(membership);

    // listening on our port
    UDPReceiver r(members.getIP(n), members.getPort(n));
    UDPSender s(members.getIP(n), members.getPort(n));
    PerfectLink p0(&s, &r, m);
    p0.send();
 
    return 0;
}
