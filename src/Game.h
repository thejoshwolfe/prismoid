#ifndef GAME_H
#define GAME_H

#include "Entity.h"

class Game
{
public:
    long long frame_counter;
    Entity * entity;
    Game();

    void doFrame(const sf::Input * input);
    void render(sf::RenderTarget * render_target);

    void saveState(std::vector<byte>* buffer);
    void loadState(std::vector<byte>::const_iterator* buffer);
};

#endif // GAME_H
