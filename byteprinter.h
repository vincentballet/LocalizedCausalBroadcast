/** @file byteprinter.h
 * @brief Target for links which prints received bytes
 * @author Sergei Volodin, EPFL
 * @date October 2018
 */

#ifndef BYTEPRINTER_H
#define BYTEPRINTER_H

#include "target.h"

/** @brief Prints received bytes */
class BytePrinter : public Target
{
public:
    BytePrinter();
    void onMessage(unsigned source, char* buffer, unsigned length);
};

#endif // BYTEPRINTER_H
