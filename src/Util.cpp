#include "Util.h"

namespace Util {


void serialize(std::vector<byte> *buffer, float value)
{
    byte * byte_pointer = reinterpret_cast<byte*>(&value);
    for (int i = 0; i < (int)sizeof(value); i++)
        buffer->push_back(byte_pointer[i]);
}
float deserializeFloat(std::vector<byte>::const_iterator* buffer)
{
    float value;
    byte * byte_pointer = reinterpret_cast<byte*>(&value);
    for (int i = 0; i < (int)sizeof(float); i++, (*buffer)++)
        byte_pointer[i] = *(*buffer);
    return value;
}

void serialize(std::vector<byte> *buffer, sf::Vector2f value)
{
    serialize(buffer, value.x);
    serialize(buffer, value.y);
}
sf::Vector2f deserializeVector2f(std::vector<byte>::const_iterator* buffer)
{
    float x = deserializeFloat(buffer);
    float y = deserializeFloat(buffer);
    return sf::Vector2f(x, y);
}

} // namespace
