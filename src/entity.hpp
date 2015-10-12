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
    // TODO: this is wasted space for static entities:
    Coord velocity;
};

#endif
