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
    MovingEntity(const Vector2 & center, const Vector2 & size, const Vector2 & velocity) :
        Entity(center, size), velocity(velocity) {}

    virtual EntityType getType() { return EntityType_MovingEntity; }

    void calculateBoundingPrismoid();

    Vector2 velocity;
    virtual const Vector2& getVelocity() const { return velocity; }
};

#endif // MOVINGENTITY_H
