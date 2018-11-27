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
#include <set>
#include "common.h"

using std::string;
using std::ifstream;
using std::map;
using std::set;
using std::pair;
using std::vector;

/** @brief Parser for membership file
 * @todo Split into Membership + Locality classes? */
class Membership
{
private:
    ifstream handle;
    map<unsigned, string> ips;
    map<unsigned, unsigned> ports;
    map<unsigned, set<unsigned>> loc;
    vector<unsigned> processes;
public:
    /**
     * @brief Load membership file
     * @param file path
     */
	Membership(string file);

    /**
     * @brief Get IP of a process
     * @param process to get IP of
     * @return IP as a string
     */
    string getIP(unsigned process);

    /**
     * @brief Get port of a process
     * @param process to get port of
     * @return Port as an int
     */
    unsigned getPort(unsigned process);

    /**
     * @brief getProcesses List of processes
     * @return Vector with processes
     */
    vector<unsigned> getProcesses();

    /**
     * @brief validProcess Checks if process ID is valid
     * @return True if valid
     */
    bool validProcess(unsigned process);

    /**
     * @brief Rank can be different than the id
     * @return Returns rank of a process
     */
    static unsigned getRank(unsigned n) {
        return n - 1;
    }
    
    /**
     * @brief getID Of a process with IP and port
     * @param IP IP address of a process
     * @param port Port of a process
     * @return 0 on failure or membership ID
     */
    unsigned getID(string IP, unsigned port);
    
    /**
     * @brief getLocality Of a process with IP and port
     * @param process to get locality of
     * @return map of processes dependance
     */
    std::set<unsigned> getLocality(unsigned process);

    /**
     * @brief Prints the locality information and membership information
     */
    void print();

};

#endif // MEMBERSHIP_H
