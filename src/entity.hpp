#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "geometry.hpp"

struct Entity {
    enum Type {
        YOU,
        WALL,
    };

    Type type;
    Rect bounds;
    Coord velocity;
};

#endif
