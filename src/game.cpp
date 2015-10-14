#include "game.hpp"

#include "input.hpp"
#include "physics.hpp"

List<Entity> entities;

void init_entities() {
    // you must be first
    entities.append({ Entity::YOU,
        {{0, -85000}, {24000, 85000}},
        {0, 0},
    });
    entities.append({ Entity::WALL,
        {{-25*32000, 0}, {51*32000, 32000}},
        {0, 0},
    });
    entities.append({ Entity::WALL,
        {{-25*32000, -25*32000}, {32000, 25*32000}},
        {0, 0},
    });
    entities.append({ Entity::WALL,
        {{25*32000, -25*32000}, {32000, 25*32000}},
        {0, 0},
    });
}

void run_the_game() {
    Coord acceleration = {0, 0};
    if (input_state[INPUT_LEFT])
        acceleration.x -= 100;
    if (input_state[INPUT_RIGHT])
        acceleration.x += 100;

    // gravity
    acceleration.y += 300;

    get_you()->velocity += acceleration;


    List<Collision> collisions;
    step_physics(&entities, &collisions);
}
