#include "common.h"

void int32ToChars(int32_t input, char *dest)
{
    dest[0] = input % 256; input /= 256;
    dest[1] = input % 256; input /= 256;
    dest[2] = input % 256; input /= 256;
    dest[3] = input % 256;
}

int32_t charsToInt32(char *input)
{
    int32_t result = 0;
    result += input[3]; result *= 256;
    result += input[2]; result *= 256;
    result += input[1]; result *= 256;
    result += input[0];
    return result;
}
