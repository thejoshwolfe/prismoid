#ifndef GAME_HPP
#define GAME_HPP

#include "list.hpp"
#include "entity.hpp"

extern List<Entity> entities;

void init_entities();
void run_the_game();

static inline Entity * get_you() {
    return &entities[0];
}

#endif
