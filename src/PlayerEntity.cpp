#include "PlayerEntity.h"

#include "Game.h"

void PlayerEntity::doController(Game *game)
{
    const sf::Input * input = game->getInput();
    const float move_acceleration = 0.5;
    const float jump_acceleration = 13.5;
    bool left = input->IsKeyDown(sf::Key::A) ||
                input->IsKeyDown(sf::Key::Left);
    bool right = input->IsKeyDown(sf::Key::D) ||
                 input->IsKeyDown(sf::Key::E) ||
                 input->IsKeyDown(sf::Key::Right);
    if (left)
        velocity.x -= move_acceleration;
    if (right)
        velocity.x += move_acceleration;
    bool was_jumping = is_jumping;
    is_jumping = input->IsKeyDown(sf::Key::Space) ||
                 input->IsKeyDown(sf::Key::W) ||
                 input->IsKeyDown(sf::Key::Comma) ||
                 input->IsKeyDown(sf::Key::Up) ||
                 input->IsKeyDown(sf::Key::K);
    if (!was_jumping && is_jumping)
        velocity.y -= jump_acceleration;
    MovingEntity::doController(game);
}
