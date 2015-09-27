#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "geometry.hpp"

struct Entity {
    Rect bounds;
    Coord velocity;
};

#endif
