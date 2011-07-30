#ifndef GAME_H
#define GAME_H

#include "Entity.h"

class Game
{
public:
    Entity * entity;

    void doFrame(const sf::Input * input);
    void render(sf::RenderTarget * render_target);

    void saveState(std::vector<byte>* buffer);
    void loadState(std::vector<byte>::const_iterator* buffer);
};

#endif // GAME_H
