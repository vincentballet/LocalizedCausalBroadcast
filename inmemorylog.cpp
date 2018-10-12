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
    // adding content to vector
    buffer.push_back(content);
}

void InMemoryLog::dump()
{
    // loop over buffer
    vector<string>::iterator it;
    for(it = buffer.begin(); it != buffer.end(); it++)
    {
        // writing data
        file << (*it) << std::endl;
    }

    // erasing dumped content from the buffer
    buffer.clear();
}
