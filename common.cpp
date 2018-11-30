#include "common.h"

InMemoryLog* memorylog = nullptr;

void int32ToChars(uint32_t input, char*dest_)
{
    uint8_t* dest = (uint8_t*) dest_;
    dest[0] = input % 256; input /= 256;
    dest[1] = input % 256; input /= 256;
    dest[2] = input % 256; input /= 256;
    dest[3] = input % 256;
}

uint32_t charsToInt32(const char* input_)
{
    uint8_t* input = (uint8_t*) input_;
    uint32_t result = 0;
    result += input[3]; result *= 256;
    result += input[2]; result *= 256;
    result += input[1]; result *= 256;
    result += input[0];
    return result;
}

void prettyprint(uint32_t* arr, unsigned length){
    std::cout << "[ ";
    std::copy(arr, arr + length, std::ostream_iterator<unsigned>(std::cout, " "));
    std::cout << "]" << std::endl;
}
