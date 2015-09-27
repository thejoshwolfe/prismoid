#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <stdint.h>

struct Coord {
    uint64_t x;
    uint64_t y;
};

struct Rect {
    Coord postion; // top left corner
    Coord size;
};

static inline Coord operator+(const Coord & a, const Coord & b) {
    return Coord{a.x + b.x, a.y + b.y};
}
static inline void operator+=(Coord & a, const Coord & b) {
    a.x += b.x;
    a.y += b.y;
}
static inline Coord operator-(const Coord & a, const Coord & b) {
    return Coord{a.x - b.x, a.y - b.y};
}
static inline void operator-=(Coord & a, const Coord & b) {
    a.x -= b.x;
    a.y -= b.y;
}

#endif
