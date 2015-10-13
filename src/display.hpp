#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include "list.hpp"
#include "entity.hpp"

void display_init();
void display_finish();
void delay_until_next_frame();
void render(List<Entity> * entities);

#endif
