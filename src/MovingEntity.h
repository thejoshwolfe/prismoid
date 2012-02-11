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
    MovingEntity(const Vector2 & center, const Vector2 & size, const sf::Color & color, float elasticity, float friction, const Vector2 & velocity) :
        Entity(true, center, size, color, elasticity, friction), velocity(velocity) {}

    virtual EntityType getType() { return EntityType_MovingEntity; }

    virtual void render(Vector2 virtual_center, sf::RenderTarget *render_target);

    virtual void doController(Game * game);

    void calculateBoundingPrismoid();

    void serialize(std::vector<byte>* buffer);
    static MovingEntity * deserialize(std::vector<byte>::const_iterator* buffer);

    // important
    Vector2 velocity;
    virtual const Vector2& getVelocity() const { return velocity; }
};

#endif // MOVINGENTITY_H
