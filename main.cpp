#include <iostream>
#include <string>
#include <stdlib.h>
#include <csignal>

using std::string;
using std::cout;
using std::endl;

void onSignalUsr1(int signal_num)
{
    if(signal_num != SIGUSR1) return;

    // Start broadcasting messages
}

void writeOutputAndHalt()
{
    // writing output file
    exit(0);
}

void onSignalInt(int signal_num)
{
    if(signal_num != SIGINT) return;
    writeOutputAndHalt();
}

void onSignalTerm(int signal_num)
{
    if(signal_num != SIGTERM) return;
    writeOutputAndHalt();
}

int main(int argc, char** argv)
{
    signal(SIGTERM, onSignalTerm);
    signal(SIGINT, onSignalInt);
    signal(SIGUSR1, onSignalUsr1);

    argc--;
    if(argc < 2)
    {
        cout << "Usage: " << argv[0] << " n membership [extra params...]" << endl;
    }
    else if(argc == 2)
    {
        int n = atoi(argv[1]);
        string membership = argv[2];
        cout << n << membership;
    }
    return 0;
}
