#include "PlayerEntity.h"

#include "Game.h"

void PlayerEntity::doController(Game *game)
{
    const sf::Input * input = game->getInput();
    const int move_acceleration = 2;
    if (input->IsKeyDown(sf::Key::W))
        velocity.y -= move_acceleration;
    if (input->IsKeyDown(sf::Key::A))
        velocity.x -= move_acceleration;
    if (input->IsKeyDown(sf::Key::S))
        velocity.y += move_acceleration;
    if (input->IsKeyDown(sf::Key::D))
        velocity.x += move_acceleration;
    MovingEntity::doController(game);
}
