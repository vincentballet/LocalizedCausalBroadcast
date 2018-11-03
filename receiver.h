/** @file receiver.h
 * @brief Receiver interface
 * @author Volodin Sergei, EPFL
 * @date October 2018
 */

#ifndef RECEIVER_H
#define RECEIVER_H

#include "target.h"
#include <pthread.h>
#include <vector>

using std::vector;

/** @class Receiver interface */
class Receiver
{
protected:
    /// @brief The target for delivering messages to
    vector<Target*> targets;

    /// @brief This process ID
    unsigned this_process;

    /// @brief Deliver a message to all targets with logical source
    void deliverToAll(unsigned source, unsigned logical_source, const char* message, unsigned length);

    /// @brief Deliver a message to all targets
    void deliverToAll(unsigned source, const char* message, unsigned length);
public:
    /**
     * @brief Receiver constructor
     * @param target The object to deliver messages to
     */
    Receiver(unsigned this_process, Target* target = nullptr);

    virtual ~Receiver() {}

    /** @brief Get this process ID */
    unsigned getThis();

    /** @brief Set target manually */
    void addTarget(Target* target);
};

#endif // RECEIVER_H
