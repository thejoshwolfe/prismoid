#ifndef GAME_H
#define GAME_H

#include "MovingEntity.h"
#include "StaticEntity.h"

class Game
{
public:
    int64 frame_counter;
    Game();

    void doFrame(const sf::Input * input);
    void render(sf::RenderTarget * render_target);

    void saveState(std::vector<byte>* buffer);
    void loadState(std::vector<byte>::const_iterator* buffer);

    const sf::Input * getInput() { return input; }

private:
    std::vector<MovingEntity *> moving_entities;
    std::vector<StaticEntity *> static_entities;
    const sf::Input * input;
};

#endif // GAME_H
