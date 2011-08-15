#ifndef GAME_H
#define GAME_H

#include "MovingEntity.h"
#include "StaticEntity.h"
#include <tr1/memory>

class Game
{
public:
    int64 frame_counter;
    Game();

    void doFrame(const sf::Input * input);
    void render(sf::RenderTarget * render_target);

    void saveState(std::vector<byte>* buffer);
    void loadState(std::vector<byte>::const_iterator* buffer);

    const sf::Input * getInput() { return input; }

private:
    // important
    std::vector<MovingEntity *> moving_entities;
    std::vector<StaticEntity *> static_entities;

    // temporary
    const sf::Input * input;

    struct Collision {
        bool valid;
        MovingEntity * entity;
        Entity * other;
        sf::Vector2f normal;
        Collision(MovingEntity * entity, Entity * other, const sf::Vector2f &normal) :
            valid(true), entity(entity), other(other), normal(normal) {}
    };
    std::multimap<Entity *, std::tr1::shared_ptr<Collision> > collisions_by_entity;
    std::priority_queue<Util::KeyAndValue<float, std::tr1::shared_ptr<Collision> > > collisions_by_time;

    void detectCollisions(MovingEntity * entity);
    bool detectCollision(MovingEntity * entity, Entity * other);
    bool maybeAddCollision(float time, MovingEntity * entity, Entity * other, const sf::Vector2f &normal);
    void doCollision(float time, std::tr1::shared_ptr<Collision> collision);
    void invalidateCollisions(MovingEntity *entity);
};

#endif // GAME_H
