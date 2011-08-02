#ifndef UTIL_H
#define UTIL_H

#include <SFML/Graphics.hpp>
#include <cmath>

typedef unsigned char byte;


namespace Util {

const float pi = 3.14159265f;
const float two_pi = 6.28318531f;

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

template <typename T>
T euclideanMod(T numerator, T denominator)
{
    T result = std::fmod(numerator, denominator);
    if (result < 0)
        result += denominator;
    return result;
}

template <typename T>
float magnitude(sf::Vector2<T> vector)
{
    return std::sqrt(vector.x * vector.x + vector.y * vector.y);
}

}

#endif
