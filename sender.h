#ifndef SENDER_H
#define SENDER_H

#include <string>

using std::string;

class Sender
{
public:
    Sender();
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
};

#endif // SENDER_H
