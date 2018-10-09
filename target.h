/** @file target.h
 * @brief This file implements the interface for all objects which have onMessage() method
 */

#ifndef TARGET_H
#define TARGET_H

/**
 * @class This class implements the interface for all objects which have onMessage() method
 */
class Target
{
public:
    /**
     * @brief Target Empty constructor
     */
    Target();

    virtual ~Target() {}

    /**
     * @brief onMessage Called when new message is available
     * @param source The sender process ID
     * @param buffer The message in memory
     * @param length The message length
     */
    virtual void onMessage(unsigned source, char* buffer, unsigned length);
};

#endif // TARGET_H
