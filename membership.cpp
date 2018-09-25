#include "membership.h"
#include <iostream>

using std::cout;
using std::endl;

Membership::Membership(std::string file)
{
    handle.open(file);

    // number of nodes
    int n;
    handle >> n;

    // loop over nodes
    for(int i = 0; i < n; i++) {
        // buffer variables
        string s;
        int n, idx;

        // number of process, IP address, port
        handle >> idx >> s >> n;

        cout << "ID " << idx << " IP " << s << " PORT " << n << endl;

        // storing data
        ips[idx] = string(s);
        ports[idx] = n;
    }
}

std::string Membership::getIP(int process)
{
    if(ips.find(process) == ips.end()) {
        cout << "Invalid process " << process << endl;
        exit(0);
    }
    return ips[process];
}

int Membership::getPort(int process)
{
    if(ports.find(process) == ports.end()) {
        cout << "Invalid process " << process << endl;
        exit(0);
    }
    return ports[process];
}
