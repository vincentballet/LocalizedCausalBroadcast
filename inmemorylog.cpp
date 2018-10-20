#include <iostream>
#include "inmemorylog.h"
#include "common.h"

using std::cout;
using std::endl;

InMemoryLog::InMemoryLog(std::string destination_filename)
{
    // opening the file
    file.open(destination_filename, std::ios::out);
}

void InMemoryLog::log(std::string content)
{
#ifdef INMEMORY_PRINT
    cout << content << endl;
#endif

    // beginning of critical section
    m.lock();

    // adding content to vector
    buffer.push_back(content);

    // end of critical section
    m.unlock();

    /// @todo What if signal happens at the moment when THIS EXACT thread is inside the critical section?
}

void InMemoryLog::dump()
{
    // locking the buffer
    m.lock();

    // loop over buffer
    vector<string>::iterator it;
    for(it = buffer.begin(); it != buffer.end(); it++)
    {
        // writing data
        file << (*it) << std::endl;
    }

    // erasing dumped content from the buffer
    buffer.clear();

    // unlocking the buffer
    m.unlock();
}
