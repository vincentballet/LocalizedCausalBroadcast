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
    int destination;
public:
    Sender(int destination);
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
    void send(char* data);

    /**
     * @brief send array of bytes
     * @param data array of bytes
     * @param N length of the array
     */
    virtual void send(char* data, int N);

    /** @brief Get target ID */
    int getTarget();
};

#endif // SENDER_H
