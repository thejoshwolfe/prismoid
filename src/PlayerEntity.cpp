#include "PlayerEntity.h"

#include "Game.h"

void PlayerEntity::doController(Game *game)
{
    const sf::Input * input = game->getInput();
    const float move_acceleration = 1.5f;
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
