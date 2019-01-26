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
#include "mysemaphore.h"

using std::string;
using std::mutex;
using std::list;

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

    /// @brief Producer-consumer semaphore, number of elements inside, 0 if the buffer is empty
    semaphore* sem_fill_count;

    /// @brief Producer-consumer semaphore, number of free cells, 0 if the buffer is full
    semaphore* sem_empty_count;

    /// Maximal number of messages
    unsigned MAX_MESSAGES;

    /// Maximal length of a message, including \0
    unsigned LMAX;

    /// Current write pointer
    volatile unsigned writeIndex = 0;

    /// Current read pointer
    volatile unsigned readIndex = 0;

    /// Should dumper thread dump data?
    volatile bool dumperActive;

    /// Buffer for data. Usage: buffer[LMAX * index + offset]
    volatile char *buffer;

    /// Buffer for data
    volatile uint64_t *timestamps;

    /// Should log() work for new messages?
    bool active;

    /// Number of iterations dump thread made
    volatile uint64_t dumpedIterations = 0;

    /// Mutex for buffer access
    mutex m;

    /// Mutex for the dumpedIterations variable
    mutex m_dump;

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

    /** @brief waitForFinishAndExit Waits until all data is in the file and closes the file
     */
    void waitForFinishAndExit();
};

#endif // INMEMORYLOG_H
