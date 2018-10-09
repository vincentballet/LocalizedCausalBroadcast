#include "failuremonitor.h"
#include <iostream>

using std::cout;
using std::endl;

FailureMonitor::FailureMonitor()
{

}

void FailureMonitor::onFailure(int process)
{
    cout << "Process " << process << " has failed" << endl;
}
