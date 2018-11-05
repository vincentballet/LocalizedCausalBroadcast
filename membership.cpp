#include "membership.h"
#include <iostream>

using std::cout;
using std::endl;
using std::map;

Membership::Membership(std::string file)
{
    handle.open(file);

    // number of nodes
    unsigned n;
    handle >> n;

    // loop over nodes
    for(unsigned i = 0; i < n; i++) {
        // buffer variables
        string s;
        unsigned n, idx;

        // number of process, IP address, port
        handle >> idx >> s >> n;

        // adding process id
        processes.push_back(idx);

        //cout << "ID " << idx << " IP " << s << " PORT " << n << endl;

        // storing data
        ips[idx] = string(s);
        ports[idx] = n;
    }
}

std::string Membership::getIP(unsigned process)
{
    if(ips.find(process) == ips.end()) {
        cout << "Invalid process " << process << endl;
        exit(0);
    }
    return ips[process];
}

unsigned Membership::getPort(unsigned process)
{
    if(ports.find(process) == ports.end()) {
        cout << "Invalid process " << process << endl;
        exit(0);
    }
    return ports[process];
}

vector<unsigned> Membership::getProcesses()
{
    return processes;
}

bool Membership::validProcess(unsigned process)
{
    vector<unsigned>::iterator it;
    for(it = processes.begin(); it != processes.end(); it++)
        if(*it == process) return true;
    return false;
}

unsigned Membership::getID(std::string IP, unsigned port)
{
    map<unsigned, string>::iterator it;
    for(it = ips.begin(); it != ips.end(); it++)
    {
        unsigned id = (*it).first;
        string ip_ = (*it).second;
        unsigned port_ = ports[id];
        if(ip_ == IP && port_ == port)
            return id;
    }
    return 0;
}
