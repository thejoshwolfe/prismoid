#ifndef UTIL_H
#define UTIL_H

#include <SFML/Graphics.hpp>

typedef unsigned char byte;


namespace Util {

template <typename T>
void serialize(std::vector<byte>* buffer, T value)
{
    byte * byte_pointer = reinterpret_cast<byte*>(&value);
    for (int i = 0; i < (int)sizeof(value); i++)
        buffer->push_back(byte_pointer[i]);
}

template <typename T>
T deserialize(std::vector<byte>::const_iterator* buffer)
{
    T value;
    byte * byte_pointer = reinterpret_cast<byte*>(&value);
    for (int i = 0; i < (int)sizeof(value); i++, (*buffer)++)
        byte_pointer[i] = *(*buffer);
    return value;
}

}

#endif
