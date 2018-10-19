#include "membership.h"
#include <iostream>

using std::cout;
using std::endl;
using std::map;

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
        //handle >> idx >> s >> n;
        handle >> s >> n;
        idx = i + 1;

        // adding process id
        processes.push_back(idx);

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

vector<int> Membership::getProcesses()
{
    return processes;
}

bool Membership::validProcess(int process)
{
    vector<int>::iterator it;
    for(it = processes.begin(); it != processes.end(); it++)
        if(*it == process) return true;
    return false;
}

int Membership::getID(std::string IP, int port)
{
    map<int, string>::iterator it;
    for(it = ips.begin(); it != ips.end(); it++)
    {
        int id = (*it).first;
        string ip_ = (*it).second;
        int port_ = ports[id];
        if(ip_ == IP && port_ == port)
            return id;
    }
    return -1;
}
