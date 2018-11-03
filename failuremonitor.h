/** @file failuremonitor.h
 * @brief Failure Monitor interface
 * @author Volodin Sergei, EPFL
 * @date October 2018
 */

#ifndef FAILUREMONITOR_H
#define FAILUREMONITOR_H

/** @brief Failure monitor interface */
class FailureMonitor
{
public:
    FailureMonitor();

    /**
     * @brief onFailure Will be called when a process has failed
     * @param process The ID of the failed process
     */
    virtual void onFailure(unsigned process);
};

#endif // FAILUREMONITOR_H
