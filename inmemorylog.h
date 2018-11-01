/** @file inmemorylog.h
 * @author Sergei Volodin, EPFL
 * @date October 2018
 * @brief This file implements the In Memory Log
 */

#ifndef INMEMORYLOG_H
#define INMEMORYLOG_H

#include <string>
#include <fstream>
#include <mutex>

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

    /// Maximal number of messages
    static const int MAX_MESSAGES = 10000000;

    /// Current number of messages in the buffer
    volatile int messages = 0;

    /// Is function log() working now?
    volatile bool active;

    /// Buffer for data
    string *buffer;

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
     * Call from ONE thread only!
     */
    void dump();
    
    /// Print only necessary stuff or not
    bool debug = false;
};

#endif // INMEMORYLOG_H
