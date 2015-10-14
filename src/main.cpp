
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
    }

    display_finish();
    return 0;
}
