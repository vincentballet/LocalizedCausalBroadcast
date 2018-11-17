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
#include <list>

using std::string;
using std::ifstream;
using std::map;
using std::pair;
using std::vector;

/** @brief Parser for membership file */
class Membership
{
private:
    ifstream handle;
    map<unsigned, string> ips;
    map<unsigned, unsigned> ports;
    vector<unsigned> processes;
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
    map<unsigned, std::list<unsigned>> getLocality(unsigned process);

};

#endif // MEMBERSHIP_H
