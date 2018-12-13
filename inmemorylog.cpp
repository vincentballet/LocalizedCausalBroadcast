#include <stdio.h>
#include <unistd.h>
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

        // incrementing iteration count
        log->dumpedIterations++;

        // stopping
        usleep(1000);
    }
}

void InMemoryLog::rollDumpLoop()
{
    // saving current number of iterations
    uint64_t currentIterations = dumpedIterations;

    // waiting for two more to ensure all data is inside the file
    while(dumpedIterations < currentIterations + 2)
    {
        fprintf(stderr, "+");
        sleep(1);
    }
}

InMemoryLog::InMemoryLog(unsigned n, string destination_filename) : n(n)
{
    // allocating memory
    buffer = new string[MAX_MESSAGES];
    timestamps = new uint64_t[MAX_MESSAGES];
    used = new bool[MAX_MESSAGES];

    // reserving data for strings
    for(int i = 0; i < MAX_MESSAGES; i++)
        const_cast<std::string&>(buffer[i]).reserve(LMAX);

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

    // spawning dumper thread
    pthread_create(&dump_thread, nullptr, &InMemoryLog::dumpLoop, this);
}

void InMemoryLog::log(std::string content)
{
    if(!active) return;
    uint64_t time = TIME_MS_NOW();

#ifdef INMEMORY_PRINT
    fprintf(stderr, "LOG %02d %07d| %lu %s\n", n, pthread_self(), time, content.c_str());
#endif

#ifdef IMMEDIATE_FILE
    fprintf(file_immediate, "%lld %s\n", time, content.c_str());
#endif

    // beginning of critical section
    m.lock();

    // obtaining write index
    int currentIndex = writeIndex;

    // complaining if all space is used
    if(used[currentIndex])
        fprintf(stderr, "WARNING: Log buffer full writeIndex = %d\n", writeIndex);

    // waiting for a variable to change...
    while(used[currentIndex]) usleep(1000);

    // incrementing write index, wrapping counter if it's at the end
    if(writeIndex == MAX_MESSAGES - 1)
        writeIndex = 0;
    else writeIndex++;

    // end of critical section
    m.unlock();

    // adding content to vector
    const_cast<std::string&>(buffer[currentIndex]) = content;
    timestamps[currentIndex] = time;

    // marking cell as used, now the reader thread will proceed
    used[currentIndex] = true;
}

void InMemoryLog::waitForFinishAndExit()
{
    // disabling new workers (so will get at most |threads| new messages)
    active = false;

    // dumping existing messages
    rollDumpLoop();

    // now disabling the dumper thread to avoid any possible issues with writing
    dumperActive = false;

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
    // dumping all existing messages to disk
    while(true)
    {
        // doing nothing if have nothing to read
        if(!used[readIndex]) break;

        // writing data
        fprintf(file, "%s\n", const_cast<std::string&>(buffer[readIndex]).c_str());

#ifdef DEBUG_FILES
        fprintf(file_ts, "%lld %s\n", timestamps[readIndex], const_cast<std::string&>(buffer[readIndex]).c_str());
#endif

        // marking cell as free and thus allowing to write to it
        used[readIndex] = false;

        // moving on
        if(readIndex == MAX_MESSAGES - 1)
            readIndex = 0;
        else readIndex++;
    }
}
