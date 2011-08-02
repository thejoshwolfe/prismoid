#include "Game.h"

Game::Game() :
    frame_counter(0)
{
    main_entity = new Entity(sf::Vector2f(0, 0), sf::Vector2f(30, 30), sf::Vector2f(0, 0), sf::Color::Blue);
    floor_entity = new Entity(sf::Vector2f(250, 305), sf::Vector2f(500, 10), sf::Vector2f(0, 0), sf::Color::Red);
}

void Game::doFrame(const sf::Input * input)
{
    frame_counter++;

    // run controllers
    const float move_acceleration = 0.4f;
    if (input->IsKeyDown(sf::Key::W)) main_entity->velocity.y -= move_acceleration;
    if (input->IsKeyDown(sf::Key::A)) main_entity->velocity.x -= move_acceleration;
    if (input->IsKeyDown(sf::Key::S)) main_entity->velocity.y += move_acceleration;
    if (input->IsKeyDown(sf::Key::D)) main_entity->velocity.x += move_acceleration;

    // determine desired motion
    main_entity->calculateMotionBoundingPolygon();

    // detect collisions

    // resolve collisions

    // apply velocity to positions
    main_entity->center.x += main_entity->velocity.x;
    main_entity->center.y += main_entity->velocity.y;
}

void Game::render(sf::RenderTarget *render_target)
{
    render_target->GetDefaultView().SetCenter(main_entity->center.x, main_entity->center.y);
    floor_entity->render(render_target);
    main_entity->render(render_target);
}

void Game::saveState(std::vector<byte>* buffer)
{
    Util::serialize(buffer, frame_counter);
    main_entity->serialize(buffer);
}
void Game::loadState(std::vector<byte>::const_iterator* buffer)
{
    frame_counter = Util::deserialize<long long>(buffer);
    delete main_entity;
    main_entity = Entity::deserialize(buffer);
}
