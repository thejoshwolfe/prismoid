#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <stdint.h>

struct Coord {
    uint64_t x;
    uint64_t y;
};

struct Rect {
    Coord postion; // top left corner
    Coord size;
};

#endif
