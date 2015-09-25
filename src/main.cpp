
#include "util.hpp"
#include "display.hpp"
#include "input.hpp"

static inline void run_the_game() {
    // TODO
}

int main() {
    display_init();

    while (!request_shutdown) {
        read_input();

        run_the_game();

        render();

        delay_until_next_frame();
    }

    display_finish();
    return 0;
}
