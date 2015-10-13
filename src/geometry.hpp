#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <stdint.h>

struct Coord {
    int64_t x; // millipixels
    int64_t y;
};

struct Rect {
    Coord position; // top left corner
    Coord size;
};

struct EdgeH {
    Coord position; // left
    int64_t size;   // points to the right
};
struct EdgeV {
    Coord position; // top
    int64_t size;   // points downward
};

static constexpr Coord operator+(const Coord & a, const Coord & b) {
    return Coord{a.x + b.x, a.y + b.y};
}
static inline void operator+=(Coord & a, const Coord & b) {
    a.x += b.x;
    a.y += b.y;
}
static constexpr Coord operator-(const Coord & a, const Coord & b) {
    return Coord{a.x - b.x, a.y - b.y};
}
static inline void operator-=(Coord & a, const Coord & b) {
    a.x -= b.x;
    a.y -= b.y;
}

static constexpr EdgeH get_top_edge(const Rect & rect) {
    return EdgeH{ {rect.position.x, rect.position.y}, rect.size.x};
}
static constexpr EdgeV get_left_edge(const Rect & rect) {
    return EdgeV{ {rect.position.x, rect.position.y}, rect.size.y};
}
static constexpr EdgeH get_bottom_edge(const Rect & rect) {
    return EdgeH{ {rect.position.x, rect.position.y + rect.size.y}, rect.size.x};
}
static constexpr EdgeV get_right_edge(const Rect & rect) {
    return EdgeV{ {rect.position.x + rect.size.x, rect.position.y}, rect.size.y};
}

#endif
