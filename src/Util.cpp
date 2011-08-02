#include "Util.h"

namespace Util {

float angleOfVector(sf::Vector2f vector)
{
    if (vector.y == 0 && vector.x == 0)
        return 0; // whatever
    return std::atan2(vector.y, vector.x);
}

float euclideanMod(float numerator, float denominator)
{
    float result = std::fmod(numerator, denominator);
    if (result < 0)
        result += denominator;
    return result;
}

int euclideanMod(int numerator, int denominator)
{
    return (numerator % denominator + denominator) % denominator;
}


}
