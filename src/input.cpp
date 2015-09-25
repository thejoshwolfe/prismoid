bool request_shutdown = false;

#include "SDL.h"

void read_input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                request_shutdown = true;
                break;
        }
    }
}
