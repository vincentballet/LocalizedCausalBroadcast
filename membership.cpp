#include "membership.h"
#include <iostream>
#include <sstream>

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

    // counter
    int c = -1;
    
    // loop over nodes
    while (std::getline(handle, line)) {
        c++;
        istringstream row(line);
        unsigned idx;
        
        if(c < n){
            // buffer variables
            string s;
            unsigned p;
            
            // process id, ip, port
            row >> idx >> s >> p;
            
            // storing data
            ips[idx] = string(s);
            ports[idx] = p;
            
            // debug
             cout << "ID " << idx << " IP " << s << " PORT " << p << endl;
        }
        else {
            // buffer variables
            unsigned local;
            std::list<unsigned> l;
            
            // process id
            row >> idx;
            
            // variable number of dependencies
            while (row >> local)
                l.push_back(local);
            
            // storing data
            loc[idx] = l;
            
            // debug
            // cout << "ID " << idx << " LIST " <<endl;
            // for (auto v : l)
            //     std::cout << "\t" << v << endl;
        }
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

map<unsigned, std::list<unsigned>> Membership::getLocality(unsigned process)
{
    return loc;
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

