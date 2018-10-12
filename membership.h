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
using std::pair;
using std::vector;

/** @brief Parser for membership file */
class Membership
{
private:
    ifstream handle;
    map<int, string> ips;
    map<int, int> ports;
    vector<int> processes;
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
     * @brief getProcesses List of processes
     * @return Vector with processes
     */
    vector<int> getProcesses();

    /**
     * @brief validProcess Checks if process ID is valid
     * @return True if valid
     */
    bool validProcess(int process);
	
    /**
     * @brief getID Of a process with IP and port
     * @param IP IP address of a process
     * @param port Port of a process
     * @return -1 on failure or membership ID
     */
    int getID(string IP, int port);
};

#endif // MEMBERSHIP_H
