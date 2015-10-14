#include "input.hpp"

#include "util.hpp"

#include <SDL.h>

bool request_shutdown = false;

bool input_state[INPUT_COUNT];
bool input_just_pressed[INPUT_COUNT];

static bool get_input_id_for_key_event(const SDL_KeyboardEvent & event, InputId * out_result) {
    switch (event.keysym.scancode) {
        case SDL_SCANCODE_W:
            *out_result = INPUT_UP;
            return true;
        case SDL_SCANCODE_A:
            *out_result = INPUT_LEFT;
            return true;
        case SDL_SCANCODE_S:
            *out_result = INPUT_DOWN;
            return true;
        case SDL_SCANCODE_D:
            *out_result = INPUT_RIGHT;
            return true;
        case SDL_SCANCODE_K:
            *out_result = INPUT_JUMP;
            return true;
        default:
            return false;
    }
}

static void handle_key_event(const SDL_KeyboardEvent & event) {
    InputId input_id;
    if (!get_input_id_for_key_event(event, &input_id))
        return;
    bool is_down = event.type == SDL_KEYDOWN;
    if (is_down) input_just_pressed[input_id] = !input_state[input_id];
    input_state[input_id] = is_down;
}

void read_input() {
    for (int i = 0; i < INPUT_COUNT; i++)
        input_just_pressed[i] = false;
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_KEYUP:
            case SDL_KEYDOWN:
                handle_key_event(event.key);
                break;
            case SDL_QUIT:
                request_shutdown = true;
                break;
        }
    }
}
