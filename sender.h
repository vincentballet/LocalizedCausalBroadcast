/** @file sender.h
 * @brief Sender interface
 * @author Volodin Sergei, EPFL
 * @date October 2018
 */

#ifndef SENDER_H
#define SENDER_H

#include <string>

using std::string;

class Sender
{
private:
    unsigned destination;
public:
    Sender(unsigned destination);
    virtual ~Sender() {}

    /**
     * @brief send a string
     * @param data (string)
     */
    void send(string data);

    /**
     * @brief send
     * @param data Null-terminated string
     */
    void send(const char* data);

    /**
     * @brief send array of bytes
     * @param data array of bytes
     * @param N length of the array
     */
    virtual void send(const char* data, unsigned N);

    /** @brief Get target ID */
    unsigned getTarget();
};

#endif // SENDER_H
