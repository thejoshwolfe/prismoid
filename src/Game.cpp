#include "Game.h"

#include "PlayerEntity.h"

Game::Game() :
    frame_counter(0)
{
    moving_entities.push_back(new PlayerEntity(sf::Vector2f(30, 0), sf::Vector2f(30, 30), sf::Color::Blue, 0.5, 1.25, sf::Vector2f(0, 0)));
    moving_entities.push_back(new MovingEntity(sf::Vector2f(30, -100), sf::Vector2f(30, 30), sf::Color::Blue, 0.5, 1.25, sf::Vector2f(0, 0)));

    static_entities.push_back(new StaticEntity(sf::Vector2f(0, 0), sf::Vector2f(20, 600), sf::Color::Red, 0.5, 0.25));
    static_entities.push_back(new StaticEntity(sf::Vector2f(250, 305), sf::Vector2f(500, 10), sf::Color::Red, 0.5, 0.25));
    static_entities.push_back(new StaticEntity(sf::Vector2f(500, 0), sf::Vector2f(20, 600), sf::Color::Red, 0.5, 0.25));
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

    std::vector<MovingEntity *> still_moving_entities = moving_entities;
    for (int asdf = 0; asdf < 10; asdf++) {
        // determine desired motion
        for (int i = 0; i < (int)still_moving_entities.size(); i++)
            still_moving_entities[i]->calculateMotionBoundingPolygon();

        // detect collisions
        for (int i = 0; i < (int)still_moving_entities.size(); i++) {
            MovingEntity * entity = still_moving_entities[i];
            for (int j = 0; j < (int)static_entities.size(); j++)
                entity->detectCollision(static_entities[j]);
            for (int j = 0; j < (int)moving_entities.size(); j++) {
                MovingEntity * other_entity = moving_entities[j];
                if (entity != other_entity)
                    entity->detectCollision(other_entity);
            }
        }

        // resolve collisions
        std::vector<MovingEntity *> next_moving_entities;
        for (int i = 0; i < (int)still_moving_entities.size(); i++) {
            MovingEntity * entity = still_moving_entities[i];
            bool needs_more = entity->resolveCollisionsAndApplyVelocity();
            if (needs_more)
                next_moving_entities.push_back(entity);
        }
        if (next_moving_entities.empty())
            break;
        still_moving_entities = next_moving_entities;
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
