/** @file main.cpp
 * @brief Main file for the Distributed Computing project
 * @date Sep 25 2018
 * @author Volodin Sergei, EPFL
 */

#include <iostream>
#include <unistd.h>
#include <string>
#include <stdlib.h>
#include <csignal>
#include "membership.h"
#include "udpreceiver.h"
#include "udpsender.h"
#include "perfectsender.h"
#include "perfectreceiver.h"
#include <time.h>
#include "common.h"
#include <list>
#include "common.h"


using std::string;
using std::cout;
using std::endl;

PerfectSender* perfectSender;
PerfectReceiver* perfectReceiver;
int m = 10;

/**
 * @brief Handle the SIGUSR1 signal
 * @param signal_num must be SIGUSR1
 */
void onSignalUsr1(int signal_num)
{
    if(signal_num != SIGUSR1) return;
	
    // Start broadcasting messages
	while(perfectSender->getNAcks() < m)
	{
		perfectSender->send(1000);
	}
	while(true)
	{
		char buf[1000];
		perfectReceiver->receive(buf, 1000);
		sleep(1);
	}
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
	perfectSender->setNAcks(INT_MAX);
	
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
	
	// Set up senders for the membership
	std::list<UDPSender> s;
	for (auto&& [key, value] : members.getIPs()) {
		if (key != n){ // should not send messages to self
			UDPSender sender(members.getIP(key), members.getPort(key));
			s.insert(s.begin(), sender);
		}
	}
	
	// Set up the perfect sender
	perfectSender = new PerfectSender(&s, &r);

	// receiving message over perfect link
	perfectReceiver = new PerfectReceiver(&r, members.getIP(n), members.getPort(n));


	// waiting for start...
	while(true)
	{
		struct timespec sleep_time;
		sleep_time.tv_sec = 0;
		sleep_time.tv_nsec = 1000;
		nanosleep(&sleep_time, NULL);
	}


    return 0;
}
