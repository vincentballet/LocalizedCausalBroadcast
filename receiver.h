#ifndef RECEIVER_H
#define RECEIVER_H

class Receiver
{
public:
    Receiver();

    virtual ~Receiver() {}

    /**
     * @brief Receive a datagram (blocking call)
     * @param data Set to a valid memory region
     * @param maxlen Maximal length to receive
     * @return Number of bytes received
     */
    virtual int receive(char* data, int maxlen);
};

#endif // RECEIVER_H
