#include "game.hpp"

#include "input.hpp"

Entity you = {{{0, 0}, {32, 96}}, {0, 0}};

void run_the_game() {
    Coord acceleration = {0, 0};
    if (input_state[INPUT_UP])
        acceleration.y -= 1;
    if (input_state[INPUT_LEFT])
        acceleration.x -= 1;
    if (input_state[INPUT_DOWN])
        acceleration.y += 1;
    if (input_state[INPUT_RIGHT])
        acceleration.x += 1;

    you.velocity += acceleration;
    you.bounds.postion += you.velocity;
}
