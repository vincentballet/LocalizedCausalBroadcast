#ifndef RECEIVER_H
#define RECEIVER_H

#include "target.h"

/** @class Receiver interface */
class Receiver
{
protected:
    /// @brief The target for delivering messages to
    Target* target;

    /// @brief This process ID
    int this_process;
public:
    /**
     * @brief Receiver constructor
     * @param target The object to deliver messages to
     */
    Receiver(int this_process, Target* target = nullptr);

    virtual ~Receiver() {}

    /** @brief Get this process ID */
    int getThis();

    /** @brief Set target manually */
    void setTarget(Target* target);
};

#endif // RECEIVER_H
