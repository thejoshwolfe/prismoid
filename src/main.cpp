
#include "display.hpp"
#include "input.hpp"
#include "physics.hpp"

static void init_entities(List<Entity> * entities) {
    entities->append({ Entity::YOU,
        {{10000, -85000 - 32000}, {24000, 85000}},
        {(-1000 - 2400), 32000},
    });

    entities->append({ Entity::WALL,
        {{-1000, 0}, {32000, 32000}},
        {0, 0},
    });
    entities->append({ Entity::WALL,
        {{4000, -3000}, {3000, 3000}},
        {0, 0},
    });
}

static void run_the_game(List<Entity> * entities) {
    Coord acceleration = {0, 0};
    if (input_state[INPUT_UP])
        acceleration.y -= 100;
    if (input_state[INPUT_LEFT])
        acceleration.x -= 100;
    if (input_state[INPUT_DOWN])
        acceleration.y += 100;
    if (input_state[INPUT_RIGHT])
        acceleration.x += 100;
    // assume you is always 0
    (*entities)[0].velocity += acceleration;

    step_physics(entities);
}

int main() {
    List<Entity> entities;

    display_init();
    init_entities(&entities);

    while (!request_shutdown) {
        read_input();

        run_the_game(&entities);

        render(&entities);

        delay_until_next_frame();
    }

    display_finish();
    return 0;
}
