#ifndef UTIL_H
#define UTIL_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <limits>

typedef unsigned char byte;
typedef int32_t int32;
typedef int64_t int64;

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

inline float angleOfVector(sf::Vector2f vector)
{
    if (vector.y == 0 && vector.x == 0)
        return 0; // whatever
    return std::atan2(vector.y, vector.x);
}

inline float euclideanMod(float numerator, float denominator)
{
    float result = std::fmod(numerator, denominator);
    if (result < 0)
        result += denominator;
    return result;
}

inline int euclideanMod(int numerator, int denominator)
{
    return (numerator % denominator + denominator) % denominator;
}

inline float magnitude(sf::Vector2f vector)
{
    return std::sqrt(vector.x * vector.x + vector.y * vector.y);
}

inline sf::Vector2f normalized(sf::Vector2f vector)
{
    float _magnitude = magnitude(vector);
    return _magnitude == 0 ? sf::Vector2f(0, 0) : vector / _magnitude;
}

template <typename T>
T dot(sf::Vector2<T> a, sf::Vector2<T> b)
{
    return a.x * b.x + a.y * b.y;
}

template <typename T>
sf::Vector2<T> perp(sf::Vector2<T> vector)
{
    return sf::Vector2<T>(vector.y, -vector.x);
}

template <typename K, typename V>
void insert(std::multimap<K, V>* map, K key, V value)
{
    map->insert(std::pair<K, V>(key, value));
}

template <typename T>
T min(T a, T b)
{
    return a < b ? a : b;
}
template <typename T>
T max(T a, T b)
{
    return a > b ? a : b;
}


}

#endif
