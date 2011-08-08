#include "Game.h"

Game::Game() :
    frame_counter(0)
{
    moving_entities.push_back(new MovingEntity(sf::Vector2f(0, 0), sf::Vector2f(30, 30), sf::Color::Blue, 0.5, 1.25, sf::Vector2f(0, 0)));
    static_entities.push_back(new StaticEntity(sf::Vector2f(250, 305), sf::Vector2f(500, 10), sf::Color::Red, 0.5, 0.25));
}

void Game::doFrame(const sf::Input * input)
{
    frame_counter++;

    for (int i = 0; i < (int)moving_entities.size(); i++)
        moving_entities[i]->resetForNextFrame();

    // run controllers
    this->input = input;
    for (int i = 0; i < (int)moving_entities.size(); i++)
        moving_entities[i]->doController(this);

    bool keep_going = true;
    while (keep_going) {
        keep_going = false;

        // determine desired motion
        for (int i = 0; i < (int)moving_entities.size(); i++)
            moving_entities[i]->calculateMotionBoundingPolygon();

        // detect collisions
        for (int i = 0; i < (int)moving_entities.size(); i++) {
            MovingEntity * entity = moving_entities[i];
            for (int j = 0; j < (int)static_entities.size(); j++)
                entity->detectCollision(static_entities[j]);
            for (int j = 0; j < (int)moving_entities.size(); j++)
                if (i != j)
                    entity->detectCollision(moving_entities[j]);
        }

        // resolve collisions
        for (int i = 0; i < (int)moving_entities.size(); i++)
            keep_going |= moving_entities[i]->resolveCollisionsAndApplyVelocity();
    }
}

void Game::render(sf::RenderTarget *render_target)
{
    render_target->GetDefaultView().SetCenter(moving_entities[0]->getCenter());

    for (int i = 0; i < (int)static_entities.size(); i++)
        static_entities[i]->render(render_target);
    for (int i = 0; i < (int)moving_entities.size(); i++)
        moving_entities[i]->render(render_target);
}

void Game::saveState(std::vector<byte>* buffer)
{
    Util::serialize(buffer, frame_counter);
    Util::serialize(buffer, (int)moving_entities.size());
    for (int i = 0; i < (int)moving_entities.size(); i++)
        moving_entities[i]->serialize(buffer);
}
void Game::loadState(std::vector<byte>::const_iterator* buffer)
{
    frame_counter = Util::deserialize<int64>(buffer);
    for (int i = 0; i < (int)moving_entities.size(); i++)
        delete moving_entities[i];
    moving_entities.clear();
    int moving_entities_size = Util::deserialize<int>(buffer);
    for (int i = 0; i < moving_entities_size; i++)
        moving_entities.push_back(MovingEntity::deserialize(buffer));
}
