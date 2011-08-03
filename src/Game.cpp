#include "Game.h"

Game::Game() :
    frame_counter(0)
{
    main_entity = new MovingEntity(sf::Vector2f(0, 0), sf::Vector2f(30, 30), sf::Color::Blue, 0.5, sf::Vector2f(0, 0));
    floor_entity = new StaticEntity(sf::Vector2f(250, 305), sf::Vector2f(500, 10), sf::Color::Red, 0.5);
}

void Game::doFrame(const sf::Input * input)
{
    frame_counter++;

    main_entity->resetTemporaryState();

    // run controllers
    const float move_acceleration = 0.5f;
    if (input->IsKeyDown(sf::Key::W)) main_entity->getVelocity()->y -= move_acceleration;
    if (input->IsKeyDown(sf::Key::A)) main_entity->getVelocity()->x -= move_acceleration;
    if (input->IsKeyDown(sf::Key::S)) main_entity->getVelocity()->y += move_acceleration;
    if (input->IsKeyDown(sf::Key::D)) main_entity->getVelocity()->x += move_acceleration;
    main_entity->getVelocity()->y += 0.4f;

    // determine desired motion
    main_entity->calculateMotionBoundingPolygon();

    // detect collisions
    main_entity->detectCollision(floor_entity);

    // resolve collisions
    main_entity->resolveCollisionsAndApplyVelocity();
}

void Game::render(sf::RenderTarget *render_target)
{
    render_target->GetDefaultView().SetCenter(main_entity->getCenter());
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
    main_entity = MovingEntity::deserialize(buffer);
}
