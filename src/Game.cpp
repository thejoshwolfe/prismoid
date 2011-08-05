#include "Game.h"

Game::Game() :
    frame_counter(0)
{
    main_entity = new MovingEntity(sf::Vector2f(0, 0), sf::Vector2f(30, 30), sf::Color::Blue, 0.5, 1.25, sf::Vector2f(0, 0));
    floor_entity = new StaticEntity(sf::Vector2f(250, 305), sf::Vector2f(500, 10), sf::Color::Red, 0.5, 0.25);
}

void Game::doFrame(const sf::Input * input)
{
    frame_counter++;

    main_entity->resetForNextFrame();

    // run controllers
    this->input = input;
    main_entity->doController(this);

    bool keep_going = true;
    while (keep_going) {
        keep_going = false;

        // determine desired motion
        main_entity->calculateMotionBoundingPolygon();

        // detect collisions
        main_entity->detectCollision(floor_entity);

        // resolve collisions
        keep_going |= main_entity->resolveCollisionsAndApplyVelocity();
    }
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
