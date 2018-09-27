/** @file membership.h
 * @brief Parser for membership file
 * @author Volodin Sergei, EPFL
 * @date Sep 25, 2018
 */

#ifndef MEMBERSHIP_H
#define MEMBERSHIP_H

#include <string>
#include <fstream>
#include <map>
#include <vector>

using std::string;
using std::ifstream;
using std::map;

/** @brief Parser for membership file */
class Membership
{
private:
    ifstream handle;
    map<int, string> ips;
    map<int, int> ports;
public:
    /**
     * @brief Load membership file
     * @param File path
     */
	Membership(string file);

    /**
     * @brief Get IP of a process
     * @param process to get IP of
     * @return IP as a string
     */
    string getIP(int process);

    /**
     * @brief Get port of a process
     * @param process to get port of
     * @return Port as an int
     */
    int getPort(int process);
	
	/**
	 * @brief Get IDs of membership
	 * @return IDs as a map
	 */
	map<int, string> getIPs();
	
	/**
	 * @brief Get Ports of membership
	 * @return Ports as a map
	 */
	map<int, int> getPorts();
};

#endif // MEMBERSHIP_H
