#ifndef MOVINGENTITY_H
#define MOVINGENTITY_H

#include "Entity.h"

class Game;

class MovingEntity : public Entity
{
public:
    enum EntityType {
        EntityType_MovingEntity,
        EntityType_PlayerEntity,
    };
    MovingEntity(const sf::Vector2f & center, const sf::Vector2f & size, const sf::Color & color, float elasticity, float friction, const sf::Vector2f & velocity) :
        Entity(center, size, color, elasticity, friction), velocity(velocity) {}

    virtual EntityType getType() { return EntityType_MovingEntity; }

    virtual void render(sf::RenderTarget *render_target);
    virtual void getBoundingPrismoid(Prismoid *prismoid);

    virtual void doController(Game * game);
    void resetForNextFrame();

    void calculateBoundingPrismoid();
    void clearCollisions() { Util::clear(&collisions); }
    void detectCollision(Entity * other);
    float getCollisionTime() { return collisions.empty() ? 1 : collisions.top().key; }
    bool moveToFirstCollision();

    void serialize(std::vector<byte>* buffer);
    static MovingEntity * deserialize(std::vector<byte>::const_iterator* buffer);

protected:
    // important
    sf::Vector2f velocity;

private:
    struct Collision {
        Entity * entity;
        sf::Vector2f normal;
        Collision(Entity * entity, const sf::Vector2f &normal)
            : entity(entity), normal(normal) {}
    };

    // temporary
    Prismoid bounding_prismoid;
    std::priority_queue<Util::KeyAndValue<float, Collision> > collisions;
    float frame_progress;

    void maybeAddCollision(float time, Entity *other, const sf::Vector2f &normal);
};

#endif // MOVINGENTITY_H
