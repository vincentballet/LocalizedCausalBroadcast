/** @file main.cpp
 * @brief Main file for the Distributed Computing project
 * @date Sep 25 2018
 * @author Volodin Sergei, EPFL
 */

#include <iostream>
#include <string>
#include <stdlib.h>
#include <csignal>
#include "membership.h"
#include "udpreceiver.h"
#include "udpsender.h"
#include <time.h>

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
    string membership = argv[2];

    // parsing membership file
    Membership m(membership);

    char buf[1000];

    UDPReceiver r("127.0.0.1", 11091);
    cout << "Receiver OK" << endl;
    UDPSender s("127.0.0.1", 11091);
    cout << "Sender OK" << endl;
    s.send("abacaba");
    cout << "Sent OK" << endl;
    cout << r.receive(buf, 100) << buf;
    cout << "Receive OK" << endl;

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
