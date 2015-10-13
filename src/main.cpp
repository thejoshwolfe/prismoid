
#include "display.hpp"
#include "input.hpp"
#include "physics.hpp"

int main() {
    display_init();
    game_init();

    while (!request_shutdown) {
        read_input();

        run_the_game();

        render();

        delay_until_next_frame();
    }

    display_finish();
    return 0;
}
