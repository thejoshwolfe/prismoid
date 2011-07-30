#include "Game.h"

static const float floor_y = 300;

Game::Game() :
    frame_counter(0), entity(NULL)
{
}

void Game::doFrame(const sf::Input * input)
{
    frame_counter++;
    const float move_acceleration = 0.4f;
    if (input->IsKeyDown(sf::Key::W)) entity->velocity.y -= move_acceleration;
    if (input->IsKeyDown(sf::Key::A)) entity->velocity.x -= move_acceleration;
    if (input->IsKeyDown(sf::Key::S)) entity->velocity.y += move_acceleration;
    if (input->IsKeyDown(sf::Key::D)) entity->velocity.x += move_acceleration;

    float gravity = 0.2;
    entity->velocity.y += gravity;
    entity->center.x += entity->velocity.x;
    entity->center.y += entity->velocity.y;
    if (entity->center.y >= floor_y) {
        // collision with ground
        entity->center.y = floor_y;
        entity->velocity.y *= -0.5;
    }
}

void Game::render(sf::RenderTarget *render_target)
{
    render_target->GetDefaultView().SetCenter(entity->center.x, entity->center.y);
    render_target->Draw(sf::Shape::Line(0, floor_y, 500, floor_y, 1, sf::Color::Red));
    entity->render(render_target);
}

void Game::saveState(std::vector<byte>* buffer)
{
    Util::serialize(buffer, frame_counter);
    entity->serialize(buffer);
}
void Game::loadState(std::vector<byte>::const_iterator* buffer)
{
    frame_counter = Util::deserializeLongLong(buffer);
    delete entity;
    entity = Entity::deserialize(buffer);
}
