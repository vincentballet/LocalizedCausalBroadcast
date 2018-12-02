#include "membership.h"
#include <iostream>
#include <sstream>
#include <cassert>

using std::cout;
using std::endl;
using std::map;
using std::istringstream;
using std::getline;

Membership::Membership(std::string file)
{
    handle.open(file);

    // buffer variable
    string line;
    
    // number of nodes
    getline(handle, line);
    istringstream row(line);
    unsigned n;
    row >> n;
    
    // loop over nodes
    for(unsigned i = 0; i < n; i++)
    {
        // reading one line
        getline(handle, line);
        istringstream row(line);

        // buffer for process id
        unsigned idx;

        // buffer variables: host and listen port
        string s;
        unsigned p;

        // process id, ip, port
        row >> idx >> s >> p;

        // must read 1, 2, 3, ...
        assert(idx == i + 1);

        // storing data
        ips[idx] = string(s);
        ports[idx] = p;

        // adding process id
        processes.push_back(idx);
    }

#ifdef LOCALIZED_CAUSAL_BROADCAST
    // second loop over nodes
    for(unsigned i = 0; i < n; i++)
    {
        // reading one line
        getline(handle, line);
        istringstream row(line);

        // buffer variables: set of dependencies, current id
        std::set<unsigned> locality;
        unsigned idx;

        // reading current id
        row >> idx;

        // must read 1, 2, 3, ...
        assert(idx == i + 1);

        // reading the row inside set l
        unsigned dependency;
        while (row >> dependency)
            locality.insert(dependency);

        // adding self to locality
        locality.insert(i+1);
        
        // saving data
        loc[idx] = locality;
    }
#endif
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

std::set<unsigned> Membership::getLocality(unsigned process)
{
    return loc[process];
}

void Membership::print()
{
    cout << " === MEMBERSHIP DATA ===" << endl;
    vector<unsigned>::iterator it;
    for(it = processes.begin(); it != processes.end(); it++)
    {
        unsigned idx = *it;
        cout << "Process #" << idx << " IP: " << ips[idx] << " Port: " << ports[idx] << " ";
        cout << "Locality: [";
        set<unsigned>::iterator it_loc;
        for(it_loc = loc[idx].begin(); it_loc != loc[idx].end(); it_loc++)
        {
            cout << *it_loc << ", ";
        }
        cout << "]" << endl;
    }
    cout << " === /MEMBERSHIP DATA ===" << endl;
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


