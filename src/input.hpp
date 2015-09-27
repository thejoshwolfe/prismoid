#ifndef INPUT_HPP
#define INPUT_HPP

enum InputId {
    INPUT_UP,
    INPUT_LEFT,
    INPUT_DOWN,
    INPUT_RIGHT,

    INPUT_COUNT,
};

extern bool input_state[INPUT_COUNT];
extern bool request_shutdown;

void read_input();

#endif
