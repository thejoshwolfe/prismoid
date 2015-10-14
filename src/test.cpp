
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
static void assert_no_overlaps(List<Entity> * entities) {
    for (int i = 0; i < entities->length(); i++) {
        for (int j = i + 1; j < entities->length(); j++) {
            assert_no_overlap((*entities)[i].bounds, (*entities)[j].bounds);
        }
    }
}
static void test_physics() {
    List<Entity> entities;

    // sliding over cracks in the sidewalk from a distance.
    entities.clear();
    entities.append({ Entity::YOU,
        {{32100, 1000}, {24000, 85000}},
        {-100, 100},
    });
    entities.append({ Entity::WALL,
        {{0, 86000}, {32000, 32000}},
        {0, 0},
    });
    entities.append({ Entity::WALL,
        {{32000, 86000}, {32000, 32000}},
        {0, 0},
    });
    step_physics(&entities);
    assert_no_overlaps(&entities);
    // still moving, but only in the x direction
    assert(entities[0].velocity.x < 0);
    assert(entities[0].velocity.y == 0);


    // sliding over cracks in the sidewalk when starting with your tow on the crack.
    entities.clear();
    entities.append({ Entity::YOU,
        {{32000, 1000}, {24000, 85000}},
        {-100, 0},
    });
    entities.append({ Entity::WALL,
        {{0, 86000}, {32000, 32000}},
        {0, 0},
    });
    entities.append({ Entity::WALL,
        {{32000, 86000}, {32000, 32000}},
        {0, 0},
    });
    step_physics(&entities);
    assert_no_overlaps(&entities);
    // still moving
    assert(entities[0].velocity.x < 0);


    // after partial-frame collision resolution,
    // fresh collision detection should find collisions that weren't going to happen before.
    entities.clear();
    entities.append({ Entity::YOU,
        {{1000, -86000}, {24000, 85000}},
        {-1100, 3000},
    });
    entities.append({ Entity::WALL,
        {{-32000, -85000 - 31000}, {32000, 32000}},
        {0, 0},
    });
    entities.append({ Entity::WALL,
        {{24000, 0}, {32000, 32000}},
        {0, 0},
    });
    step_physics(&entities);
    assert_no_overlaps(&entities);
    // not still moving left; banged head against surprise wall.
    assert(entities[0].velocity.x == 0);


    // after partial-frame collision resolution,
    // fresh collision detection should not detect collisions before the last time of impact.
    entities.clear();
    entities.append({ Entity::YOU,
        {{10000, -85000 - 32000}, {24000, 85000}},
        {(-10000 - 24000), 32000},
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
    assert_no_overlaps(&entities);
    // still moving left; not run into the tiny wall
    assert(entities[0].velocity.x < 0);
}

int main() {
    test_physics();
    return 0;
}
