#include <stdio.h>
#include "inmemorylog.h"
#include "common.h"
#include <unistd.h>
#include <cassert>

using std::endl;

void *InMemoryLog::dumpLoop(void *arg)
{
    InMemoryLog* memorylog = (InMemoryLog*) arg;

    // dumping data continuously
    while(true)
    {
        // exit when not active
        if(!memorylog->active)
            return nullptr;

        int dumped = memorylog->dump();

        // waiting 1 ms if there are no new messages
        if(dumped == 0)
            usleep(1000);
    }

    // never returns if always active
}

InMemoryLog::InMemoryLog(unsigned n, string destination_filename) : n(n)
{
    // maximal number of messages in the buffer
    MAX_MESSAGES = 100000;

    // allocating memory
    buffer = new string[MAX_MESSAGES];
    timestamps = new uint64_t[MAX_MESSAGES];

    // sanity check
    assert(buffer);
    assert(timestamps);

    // currently, active
    active = true;

    // opening the file
    file.open(destination_filename, std::ios::out);

#ifdef DEBUG_FILES
    // opening the file
    file_ts.open(destination_filename + ".ts", std::ios::out);
#endif

#ifdef IMMEDIATE_FILE
    // opening the immediate file output
    file_immediate.open(destination_filename + ".nowait", std::ios::out);
#endif

#ifdef INMEMORY_PRINT
    // Logging the beginning
    log("BEGINNING");
#endif


    // initializing semaphores
    sem_init(&full_sem, 0, MAX_MESSAGES);
    sem_init(&empty_sem, 0, 0);

    // starting the thread for dumping data
    pthread_create(&dump_thread, nullptr, &InMemoryLog::dumpLoop, this);
}

InMemoryLog::~InMemoryLog()
{
    file.close();
}

void InMemoryLog::log(std::string content)
{
    if(!active) return;
    uint64_t time = TIME_MS_NOW();

#ifdef INMEMORY_PRINT
    fprintf(stderr, "LOG %02d %07d| %lu %s\n", n, pthread_self(), time, content.c_str());
#endif

#ifdef IMMEDIATE_FILE
    file_immediate << time << " " << content << endl;
#endif

    //sem_wait(&full_sem);

    // beginning of critical section
    m_write.lock();

    /// Using a ring buffer
    /// [....mmmm..]
    ///          ^ next write pointer (for logging)
    ///      ^ next read pointer (for dumping)
    ///
    /// [mm...mmm]
    ///    ^ next write pointer (for logging)
    ///       ^ next read pointer (for dumping)

    // if buffer is full, dumping data in the worker thread
    int current_read_index = read_index;
    if(current_read_index == MAX_MESSAGES)
        current_read_index = 0;

    if((current_read_index == 0 && write_index == MAX_MESSAGES - 1) || write_index == current_read_index - 1)
    {
        printf("WARNING: dumping data from the worker thread to avoid data loss. Consider increasing the buffer size\n");
        printf("Current_read_index %d write index %d\n", current_read_index, write_index);
        dump();
    }

    //printf("Current write index %d Current read index %d\n", write_index, read_index);

    // adding data
    buffer[write_index] = content;
    timestamps[write_index] = time;
    write_index++;

    // starting to write from the beginning
    if(write_index == MAX_MESSAGES)
        write_index = 0;

    // end of critical section
    m_write.unlock();

    //sem_post(&empty_sem);
}

int InMemoryLog::dump(bool last)
{
    //sem_wait(&empty_sem);

    m_read.lock();

    // getting current number of messages
    // DONT CARE if there are writers right now
    int current_write_index = write_index;

    // can read intermediate value for write_index
    if(current_write_index == MAX_MESSAGES)
        current_write_index = 0;

    // number of dumped messages
    int dumped = 0;

    // loop over buffer
    for(; ; read_index++)
    {
        // going to the beginning
        if(read_index == MAX_MESSAGES)
            read_index = 0;

        // if have i at an empty element
        if(read_index == current_write_index)
            break;

        // writing data
        file << buffer[read_index] << std::endl;
        //sem_post(&full_sem);
        dumped++;

#ifdef DEBUG_FILES
        file_ts << timestamps[read_index] << " " << buffer[read_index] << std::endl;
#endif
    }

    // at last iteration: close the file
    // and do not unlock the mutex
    if(last)
    {
        printf("%s", "Dumped all remaining messages. Closing the log file...\n");
        close();
    }
    // otherwise, allow for subsequent dump() calls
    else
    {
        //printf("Dumped %d messages up to %d, continuing...\n", dumped, current_write_index);
        m_read.unlock();
    }

    return dumped;
}

void InMemoryLog::close()
{
    file.close();
}

void InMemoryLog::disable()
{
    m_write.lock();
    active = false;
}
