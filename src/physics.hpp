#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include "list.hpp"
#include "entity.hpp"
#include "rat64.hpp"

struct Collision {
    enum Orientation {
        UP    = 1,
        LEFT  = 2,
        DOWN  = 4,
        RIGHT = 8,
        UP_LEFT    = UP    | LEFT,
        LEFT_DOWN  = LEFT  | DOWN,
        DOWN_RIGHT = DOWN  | RIGHT,
        RIGHT_UP   = RIGHT | UP,
    };

    rat64 time;
    Orientation orientation;
    int entity_index1;
    int entity_index2;
};

void step_physics(List<Entity> * entities, List<Collision> * collisions);

#endif
