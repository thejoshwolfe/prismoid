#ifndef UTIL_H
#define UTIL_H

#include <SFML/Graphics.hpp>

typedef unsigned char byte;


namespace Util {
void serialize(std::vector<byte>* buffer, float value);
float deserializeFloat(std::vector<byte>::const_iterator* buffer);

void serialize(std::vector<byte>* buffer, long long value);
long long deserializeLongLong(std::vector<byte>::const_iterator* buffer);

void serialize(std::vector<byte>* buffer, sf::Vector2f value);
sf::Vector2f deserializeVector2f(std::vector<byte>::const_iterator* buffer);

}

#endif
