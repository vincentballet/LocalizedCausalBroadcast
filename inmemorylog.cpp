#include <stdio.h>
#include "inmemorylog.h"
#include "common.h"

using std::endl;

void *InMemoryLog::dumpLoop(void *arg)
{
    InMemoryLog* memorylog = (InMemoryLog*) arg;

    // dumping data continuously
    while(true)
    {
        memorylog->dump();
    }

    // never returns
}

InMemoryLog::InMemoryLog(unsigned n, string destination_filename) : n(n)
{
    // allocating memory
    buffer = new string[MAX_MESSAGES];
    timestamps = new uint64_t[MAX_MESSAGES];

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

    sem_wait(&full_sem);

    // beginning of critical section
    m.lock();

    /// Using a ring buffer
    /// [....mmmm..]
    ///          ^ next write pointer (for logging)
    ///      ^ next read pointer (for dumping)
    ///
    /// [mm...mmm]
    ///    ^ next write pointer (for logging)
    ///       ^ next read pointer (for dumping)

    if(write_index == read_index - 1)
    {
        // WARNING: dropping message
        fprintf(stderr, "DROP   | %lu %s\n", time, content.c_str());
        m.unlock();
        return;
    }

    // adding data
    buffer[write_index] = content;
    timestamps[write_index] = time;
    write_index++;

    // starting to write from the beginning
    if(write_index == MAX_MESSAGES)
        write_index = 0;

    // end of critical section
    m.unlock();

    sem_post(&empty_sem);
}

void InMemoryLog::dump()
{
    sem_wait(&empty_sem);

    // not active (so there are no new messages)
    active = false;

    // getting current number of messages
    // DONT CARE if there are writers right now
    int current_write_index = write_index;

    // loop over buffer
    int i;
    for(i = read_index; ; i++)
    {
        // going to the beginning
        if(i == MAX_MESSAGES)
            i = 0;

        // if have i at an empty element
        if(i == current_write_index)
            break;

        // writing data
        file << buffer[i] << std::endl;

#ifdef DEBUG_FILES
        file_ts << timestamps[i] << " " << buffer[i] << std::endl;
#endif
    }

    // always have i = next element to read
    read_index = i;
    if(read_index == MAX_MESSAGES)
        read_index = 0;

#ifdef INMEMORY_PRINT
    // logging end
    fprintf(stderr, "END\n");
    file << "END" << std::endl;
#endif

    sem_post(&full_sem);
}
