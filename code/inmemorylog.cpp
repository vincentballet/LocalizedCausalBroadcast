#include <stdio.h>
#include <unistd.h>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include "inmemorylog.h"
#include "common.h"

using std::endl;

void *InMemoryLog::dumpLoop(void *arg)
{
    InMemoryLog* log = (InMemoryLog*) arg;
    while(true)
    {
        // not active -> not dumping
        if(log->dumperActive) log->dump();

        // critical section
        log->m_dump.lock();

        // incrementing iteration count
        log->dumpedIterations++;

        // end of critical section
        log->m_dump.unlock();

        // stopping
        usleep(1000);
    }
}

void InMemoryLog::rollDumpLoop()
{
    // critical section
    m_dump.lock();

    // saving current number of iterations
    uint64_t oldIterations = dumpedIterations;

    // end of critical section
    m_dump.unlock();

    // waiting for two more to ensure all data is inside the file
    while(true)
    {
        // critical section
        m_dump.lock();

        // current number of iterations
        uint64_t newIterations = dumpedIterations;

        // end of critical section
        m_dump.unlock();

        // exiting after two more iterations
        if(newIterations >= oldIterations + 2) return;

        fprintf(stderr, "+");
        sleep(1);
    }
}

InMemoryLog::InMemoryLog(unsigned n, string destination_filename) : n(n)
{
    // setting up the limits
    MAX_MESSAGES = 1000001;
    LMAX = 256;

    // allocating memory
    buffer = new char[MAX_MESSAGES * LMAX];
    timestamps = new uint64_t[MAX_MESSAGES];

    // sanity checks
    assert(buffer);
    assert(timestamps);

    // currently, active
    active = true;
    dumperActive = true;

    // opening the file
    file = fopen(destination_filename.c_str(), "w");

#ifdef DEBUG_FILES
    // opening the file
    file_ts = fopen((destination_filename + ".ts").c_str(), "w");
#endif

#ifdef IMMEDIATE_FILE
    // opening the immediate file output
    file_immediate = fopen((destination_filename + ".nowait").c_str(), "w");
#endif

    // initializing semaphores
    sem_fill_count = new semaphore(0);
    sem_empty_count = new semaphore(MAX_MESSAGES);

    // spawning dumper thread
    pthread_create(&dump_thread, nullptr, &InMemoryLog::dumpLoop, this);
}

void InMemoryLog::log(std::string content)
{
    uint64_t time = TIME_MS_NOW();

#ifdef INMEMORY_PRINT
    fprintf(stderr, "LOG %02d %07d| %lu %s\n", n, pthread_self(), time, content.c_str());
#endif

#ifdef IMMEDIATE_FILE
    fprintf(file_immediate, "%lld %s\n", time, content.c_str());
#endif

    // length to write
    unsigned len = content.length() + 1;

    // checking if the input fits
    if(len > LMAX)
    {
        printf("Log string %s is too large. It will not be logged.\n", content.c_str());
        return;
    }

    // not active -> doing nothing
    if(!active) return;

    // decreasing number of free cells
    sem_empty_count->wait();

    // beginning of critical section
    m.lock();

    // adding content to vector
    memcpy((void*) (buffer + LMAX * writeIndex), content.c_str(), len);

    // saving timestamp
    timestamps[writeIndex] = time;

    // incrementing write index, wrapping counter if it's at the end
    if(writeIndex == MAX_MESSAGES - 1)
        writeIndex = 0;
    else writeIndex++;

    // end of critical section
    m.unlock();

    // increasing the number of used cells
    sem_fill_count->notify();
}

void InMemoryLog::waitForFinishAndExit()
{
    // no more workers, will get at most |threads| more messages
    active = false;

    // dumping existing messages
    rollDumpLoop();

    // critical section
    m_dump.lock();

    // now disabling the dumper thread to avoid any possible issues with writing
    dumperActive = false;

    // end of critical section
    m_dump.unlock();

    // waiting for more iterations to ensure all writes are done
    rollDumpLoop();

    // no more writes and all messages prior to active=false are dumped => closing the file
    fclose(file);

#ifdef DEBUG_FILES
    fclose(file_ts);
#endif

    // rollDumpLoop prints progress, switching to new line
    fprintf(stderr, "\n");
}

void InMemoryLog::dump()
{
    // allocating buffer for data
    char *buf = new char[LMAX];

    // dumping all existing messages to disk
    while(true)
    {
        // waiting when have the element filled
        // or returning if the buffer is empty
        if(sem_fill_count->try_wait() == false)
            break;

        // locking to allow for memory synchronization
        m.lock();

        // copying data from the buffer
        memcpy(buf, (void*) (buffer + LMAX * readIndex), LMAX);

#ifdef DEBUG_FILES
        // copying current timestamp
        uint64_t timestamp = timestamps[readIndex];
#endif

        // moving on
        if(readIndex == MAX_MESSAGES - 1)
            readIndex = 0;
        else readIndex++;

        // end of critical section
        m.unlock();

        // marking the dumped cell as empty
        sem_empty_count->notify();

        // writing data
        fprintf(file, "%s\n", buf);

#ifdef DEBUG_FILES
        fprintf(file_ts, "%lld %s\n", timestamp, buf);
#endif
    }

    // freeing data
    free(buf);
}
