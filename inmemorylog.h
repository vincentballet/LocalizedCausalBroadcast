/** @file inmemorylog.h
 * @author Sergei Volodin, EPFL
 * @date October 2018
 * @brief This file implements the In Memory Log
 */

#ifndef INMEMORYLOG_H
#define INMEMORYLOG_H

class ImMemoryLog;

#include <string>
#include <fstream>
#include <mutex>
#include <semaphore.h>
#include <pthread.h>

using std::string;
using std::ofstream;
using std::mutex;

/** @class This class implements the In Memory Log */
class InMemoryLog
{
private:
    /// Semaphore with N - number of messages
    sem_t full_sem;

    /// Semaphore with number of messages
    sem_t empty_sem;

    /// Dump thread
    pthread_t dump_thread;

    /// Output file
    ofstream file;

    /// Output file with timestamps
    ofstream file_ts;

    /// Output file (immediate)
    ofstream file_immediate;

    /// Maximal number of messages between dumps
    static const int MAX_MESSAGES = 1000000;

    /// Using a ring buffer
    /// [mm...mmm]
    ///    ^ next write pointer (for logging)
    ///       ^ next read pointer (for dumping)

    /// Next write pointer
    volatile int write_index = 0;

    /// Next read pointer
    volatile int read_index = 0;

    /// Is function log() working now?
    volatile bool active;

    /// Buffer for data
    string *buffer;

    /// Buffer for time
    uint64_t *timestamps;

    /// Mutex for buffer access
    mutex m;

    /// @brief Process ID
    unsigned n;

    /**
     * @brief dumpLoop Dumps the data to the file continuously
     * @param arg Instance of InMemoryLog
     * @return never returns
     */
    static void* dumpLoop(void* arg);
public:
    /**
     * @brief InMemoryLog initializer
     * @param destination_filename The file to write
     */
    InMemoryLog(unsigned n, string destination_filename);

    /** @brief Closes the file */
    ~InMemoryLog();

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

    /** @brief Close the file */
    void close();
};

#endif // INMEMORYLOG_H
