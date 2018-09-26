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

using std::string;
using std::cout;
using std::endl;

/**
 * @brief Handle the SIGUSR1 signal
 * @param signal_num must be SIGUSR1
 */
void onSignalUsr1(int signal_num)
{
    if(signal_num != SIGUSR1) return;
	
    // Start broadcasting messages
	// Each process starts broadcasting messages
	// Content of messages : 1 ... m
	// Each time a process sends a message m, we store "b m\n"
	// Each time a process receives a message m from process q, we store "d q m\n"
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
 * @return
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
	int m = 10;
	string membership = argv[2];
	
	// extra params (expected to be the # of messages)
	// default : 10
	if (argc > 2){ m = atoi(argv[3]); }

	// check the parameters
	assert(1 <= n && n <= 5 && m > 0);
	
    // parsing membership file
    Membership members(membership);

    // listening on our port
    UDPReceiver r(members.getIP(n), members.getPort(n));

    // sending message over perfect link
    if(n == 1) {
        PerfectSender sender(members.getIP(2), members.getPort(2), &r);
        cout << "Created Sender" << endl;
		
        while(sender.getNAcks() < m)
        {
            sender.send(1000);
        }
    }

    // receiving message over perfect link
    else if(n == 2) {
        char buf[1000];
        PerfectReceiver receiver(&r, members.getIP(1), members.getPort(1));
        cout << "Created receiver" << endl;
        while(true)
        {
            int len = receiver.receive(buf, 1000);
            cout << "Received [" << charsToInt32(buf) << "], " << len << " bytes!" << endl;
            sleep(1);
        }
    }

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
