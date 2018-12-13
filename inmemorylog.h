/** @file inmemorylog.h
 * @author Sergei Volodin, EPFL
 * @date October 2018
 * @brief This file implements the In Memory Log
 */

#ifndef INMEMORYLOG_H
#define INMEMORYLOG_H

class ImMemoryLog;

#include <string>
#include <mutex>
#include <list>
#include <pthread.h>
#include <stdio.h>

using std::string;
using std::mutex;
using std::list;

#define LMAX 255

/** @class This class implements the In Memory Log */
class InMemoryLog
{
private:

    /// Output file
    FILE* file;

    /// Output file with timestamps
    FILE* file_ts;

    /// Output file (immediate)
    FILE* file_immediate;

    /// Maximal number of messages
    static const int MAX_MESSAGES = 1000000;

    /// Current write pointer
    volatile int writeIndex = 0;

    /// Current read pointer
    volatile int readIndex = 0;

    /// Is function log() working now?
    volatile bool active;

    /// Should dumper thread dump data?
    volatile bool dumperActive;

    /// Buffer for data
    volatile string *buffer;

    /// Buffer for data
    volatile uint64_t *timestamps;

    /// Is a cell used by some message?
    volatile bool* used;

    /// Number of iterations dump thread made
    volatile uint64_t dumpedIterations = 0;

    /// Mutex for buffer access
    mutex m;

    /// @brief Process ID
    unsigned n;

    /** Dump thread loop */
    static void* dumpLoop(void* arg);

    /// Dump thread
    pthread_t dump_thread;

    /** Wait for two iterations of dump loop, thus guaranteeing one complete total pass */
    void rollDumpLoop();

    /**
     * @brief Dump all data to file from memory
     * Call from ONE thread only!
     */
    void dump();
public:
    /**
     * @brief InMemoryLog initializer
     * @param destination_filename The file to write
     */
    InMemoryLog(unsigned n, string destination_filename);

    /**
     * @brief log Logs a string
     * @param content The string to log
     */
    void log(string content);

    /** Waits until all data is in the file and closes the file */
    void waitForFinishAndExit();
};

#endif // INMEMORYLOG_H
