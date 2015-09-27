#include "game.hpp"

#include "input.hpp"

Rect you_bounds = {{0, 0}, {32, 96}};

void run_the_game() {
    if (input_state[INPUT_UP])
        you_bounds.postion.y -= 1;
    if (input_state[INPUT_LEFT])
        you_bounds.postion.x -= 1;
    if (input_state[INPUT_DOWN])
        you_bounds.postion.y += 1;
    if (input_state[INPUT_RIGHT])
        you_bounds.postion.x += 1;
}
