/** @file inmemorylog.h
 * @author Sergei Volodin, EPFL
 * @date October 2018
 * @brief This file implements the In Memory Log
 */

#ifndef INMEMORYLOG_H
#define INMEMORYLOG_H

#include <vector>
#include <string>
#include <fstream>
#include <mutex>

using std::vector;
using std::string;
using std::ofstream;
using std::mutex;

/** @class This class implements the In Memory Log */
class InMemoryLog
{
private:
    /// Output file
    ofstream file;

    /// Output file (immediate)
    ofstream file_immediate;

    /// Output in-memory buffer
    vector<string> buffer;

    /// Mutex for buffer access
    mutex m;
public:
    /**
     * @brief InMemoryLog initializer
     * @param destination_filename The file to write
     */
    InMemoryLog(string destination_filename);

    /**
     * @brief log Logs a string
     * @param content The string to log
     */
    void log(string content);

    /**
     * @brief Dump all data to file from memory
     */
    void dump();
};

#endif // INMEMORYLOG_H
