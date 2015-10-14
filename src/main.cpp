
#include "display.hpp"
#include "input.hpp"
#include "game.hpp"

int main() {
    display_init();
    init_entities();

    while (!request_shutdown) {
        read_input();

        run_the_game();

        render(&entities);

        delay_until_next_frame();
    }

    display_finish();
    return 0;
}
