#include <iostream>
#include "inmemorylog.h"
#include "common.h"

using std::cout;
using std::endl;

InMemoryLog::InMemoryLog(std::string destination_filename)
{
    // allocating memory
    buffer = new string[MAX_MESSAGES];

    // currently, active
    active = true;

    // opening the file
    file.open(destination_filename, std::ios::out);

#ifdef INMEMORY_PRINT
    // opening the immediate file output
    file_immediate.open(destination_filename + ".nowait", std::ios::out);
#endif

    // Logging the beginning
    log("BEGINNING");
}

void InMemoryLog::log(std::string content)
{
    if(!active) return;
#ifdef INMEMORY_PRINT
    cout << "LOG   | " << content << endl;
    file_immediate << content << endl;
#endif

    // beginning of critical section
    m.lock();

    if(messages < MAX_MESSAGES)
    {
        // adding content to vector
        buffer[messages] = content;
        messages++;
    }
    else
    {
        // WARNING: dropping message
        cout << "Message Dropped: " << content << endl;
    }

    // end of critical section
    m.unlock();
}

void InMemoryLog::dump()
{
    // not active (so there are no new messages)
    active = false;

    // getting current number of messages
    // DONT CARE if there are writers right now
    int current_messages = messages;

    // loop over buffer
    for(int i = 0; i < current_messages; i++)
    {
        // writing data
        file << buffer[i] << std::endl;
    }

    // logging end
    cout << "END" << std::endl;
    file << "END" << std::endl;

    file.close();
}
