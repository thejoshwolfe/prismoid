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
        {{-10*32000, 0}, {21*32000, 32000}},
        {0, 0},
    });
    entities.append({ Entity::WALL,
        {{-10*32000, -10*32000}, {32000, 10*32000}},
        {0, 0},
    });
    entities.append({ Entity::WALL,
        {{10*32000, -10*32000}, {32000, 10*32000}},
        {0, 0},
    });
}

static bool is_on_ground = false;

void run_the_game() {
    Coord acceleration = {0, 0};
    if (input_state[INPUT_LEFT])
        acceleration.x -= 250;
    if (input_state[INPUT_RIGHT])
        acceleration.x += 250;

    // jomp
    if (is_on_ground && input_state[INPUT_UP])
        acceleration.y -= 10000;

    // gravity
    acceleration.y += 250;

    get_you()->velocity += acceleration;


    List<Collision> collisions;
    step_physics(&entities, &collisions);

    is_on_ground = false;
    for (int i = 0; i < collisions.length(); i++) {
        const Collision & collision = collisions[i];
        if (entities[collision.entity_index1].type == Entity::YOU &&
            entities[collision.entity_index2].type == Entity::WALL &&
            collision.orientation == Collision::DOWN)
        {
            is_on_ground = true;
            break;
        }
    }
}
