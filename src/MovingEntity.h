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
        Entity(true, center, size, color, elasticity, friction), velocity(velocity) {}

    virtual EntityType getType() { return EntityType_MovingEntity; }

    virtual void render(sf::RenderTarget *render_target);

    virtual void doController(Game * game);

    void calculateBoundingPrismoid();

    void serialize(std::vector<byte>* buffer);
    static MovingEntity * deserialize(std::vector<byte>::const_iterator* buffer);

    // important
    sf::Vector2f velocity;
    virtual const sf::Vector2f& getVelocity() const { return velocity; }
};

#endif // MOVINGENTITY_H
