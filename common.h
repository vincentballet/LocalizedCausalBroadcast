/** @file common.h
 * @brief Int to chars conversion
 * @author Volodin Sergei, EPFL
 * @date October 2018
 */

#ifndef COMMON_H
#define COMMON_H

#include <inttypes.h>

/** @macro minimal of two numbers */
#define min(x, y) ((x) > (y) ? (y) : (x))

/**
 * @brief int32ToChars Convert unsigned int32 to chars
 * @param input int
 * @param dest chars
 */
void int32ToChars(uint32_t input, char* dest);

/**
 * @brief charsToInt32 Convert chars to uint32
 * @param input chars
 * @return uint32
 */
uint32_t charsToInt32(char* input);

#endif // COMMON_H
