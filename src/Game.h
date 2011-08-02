#ifndef GAME_H
#define GAME_H

#include "MovingEntity.h"
#include "StaticEntity.h"

class Game
{
public:
    long long frame_counter;
    Game();

    void doFrame(const sf::Input * input);
    void render(sf::RenderTarget * render_target);

    void saveState(std::vector<byte>* buffer);
    void loadState(std::vector<byte>::const_iterator* buffer);

private:
    MovingEntity * main_entity;
    StaticEntity * floor_entity;

};

#endif // GAME_H
