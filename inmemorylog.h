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
    /// Dump thread
    pthread_t dump_thread;

    /// Output file
    ofstream file;

    /// Output file with timestamps
    ofstream file_ts;

    /// Output file (immediate)
    ofstream file_immediate;

    /// @brief The lock for cond wait
    pthread_mutex_t lock;

    /// @brief The wait condition to wake up the dumper thread
    pthread_cond_t cond;

    /// Maximal number of messages between dumps
    int MAX_MESSAGES;

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
    mutex m_write, m_read;

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
     * @param last Set to true to close the file after writing
     * @returns Number of dumped messages
     */
    int dump(bool last = false);

    /** @brief Close the file */
    void close();

    /**
     * @brief disable Make subsequent log() calls do nothing
     */
    void disable();
};

#endif // INMEMORYLOG_H
