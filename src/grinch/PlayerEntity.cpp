#include "PlayerEntity.h"

#include "Game.h"

void PlayerEntity::doController(Game *game)
{
    const sf::Input * input = game->getInput();
    const float move_acceleration = 0.1;
    const float jump_acceleration = 13.5;
    bool left = input->IsKeyDown(sf::Key::A) ||
                input->IsKeyDown(sf::Key::Left);
    bool right = input->IsKeyDown(sf::Key::D) ||
                 input->IsKeyDown(sf::Key::Right);
    if (left)
        velocity.x -= move_acceleration;
    if (right)
        velocity.x += move_acceleration;

    if (input->IsKeyDown(sf::Key::W))
        velocity.y -= move_acceleration;
    if (input->IsKeyDown(sf::Key::S))
        velocity.y += move_acceleration;

    bool was_jumping = is_jumping;
    is_jumping = input->IsKeyDown(sf::Key::Space) ||
                 input->IsKeyDown(sf::Key::K);
    if (!was_jumping && is_jumping)
        velocity.y -= jump_acceleration;
}

void PlayerEntity::render(Vector2 virtual_center, sf::RenderTarget * render_target)
{
    sf::Shape shape = sf::Shape();
    std::vector<Vector2> polygon;
    Util::makeRectangle(&polygon, center, size);
    for (int i = 0; i < (int)polygon.size(); i++)
        shape.AddPoint(Util::toRenderPoint(virtual_center, polygon[i]), sf::Color::Blue);
    render_target->Draw(shape);
}
