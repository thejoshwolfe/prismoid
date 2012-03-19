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
    MovingEntity(const Vector2 & center, const Vector2 & size, const sf::Color & color) :
        Entity(true, center, size, color, 0.1, 1.25) {}

    virtual EntityType getType() { return EntityType_MovingEntity; }

    virtual void render(Vector2 virtual_center, sf::RenderTarget *render_target);

    virtual void doController(Game * game);

    void calculateBoundingPrismoid();

    Vector2 velocity;
    virtual const Vector2& getVelocity() const { return velocity; }
};

#endif // MOVINGENTITY_H
