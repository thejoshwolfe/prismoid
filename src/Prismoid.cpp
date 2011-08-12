#include "Prismoid.h"

#include "Util.h"

sf::Vector2f Prismoid::getNormal(int i)
{
    // assumes clockwise, or something
    return Util::perp(bases[0][(i + 1) % size()] - bases[0][i]);
}
