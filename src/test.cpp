
#include "physics.hpp"

#include <stdio.h>
#include <stdlib.h>

#define fail(msg) _fail(__FILE__, __LINE__, __func__, (msg))
static void _fail(const char * filename, int line_number, const char * function_name, const char * msg) {
    // gcc compile error format, so that eclipse will give us red squiggly underlines where the fail was called.
    fprintf(stderr, "%s:%d: %s: %s\n", filename, line_number, function_name, msg);
    abort();
}

#define assert(condition) _assert((condition), __FILE__, __LINE__, __func__)
static void _assert(bool condition, const char * filename, int line_number, const char * function_name) {
    if (!condition)
        _fail(filename, line_number, function_name, "assertion failure");
}

static void assert_no_overlap(const Rect & a, const Rect & b) {
    int horizontal_overlap = get_line_segment_overlap(a.position.x, a.position.x + a.size.x, b.position.x, b.position.x + b.size.x);
    int vertical_overlap   = get_line_segment_overlap(a.position.y, a.position.y + a.size.y, b.position.y, b.position.y + b.size.y);
    if (horizontal_overlap * vertical_overlap == 0) {
        // at least one of the dimensions is overlapping. they better not both be.
        assert(horizontal_overlap + vertical_overlap != 0);
    }
}
static void assert_no_overlaps(const List<Entity> & entities) {
    for (int i = 0; i < entities.length(); i++) {
        for (int j = i + 1; j < entities.length(); j++) {
            assert_no_overlap(entities[i].bounds, entities[j].bounds);
        }
    }
}
static void test_physics() {
    List<Entity> entities;

    // after partial-frame collision resolution,
    // fresh collision detection should not detect collisions before the last time of impact.
    entities.clear();
    entities.append({ Entity::YOU,
        {{10000, -85000 - 32000}, {24000, 85000}},
        {(-1000 - 2400), 32000},
    });
    entities.append({ Entity::WALL,
        {{-1000, 0}, {32000, 32000}},
        {0, 0},
    });
    entities.append({ Entity::WALL,
        {{4000, -3000}, {3000, 3000}},
        {0, 0},
    });
    step_physics(&entities);
    assert_no_overlaps(entities);
    // still moving left; not run into the tiny wall
    assert(entities[0].velocity.x < 0);
}

int main() {
    test_physics();
}
