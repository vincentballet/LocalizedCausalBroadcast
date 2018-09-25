#ifndef MEMBERSHIP_H
#define MEMBERSHIP_H

#include <string>
#include <fstream>
#include <map>

using std::string;
using std::ifstream;
using std::map;

class Membership
{
private:
    ifstream handle;
    map<int, string> ips;
    map<int, int> ports;
public:
    Membership(string file);
    string getIP(int process);
    int getPort(int process);
};

#endif // MEMBERSHIP_H
