#ifndef PLAYERENTITY_H
#define PLAYERENTITY_H

#include "MovingEntity.h"

class PlayerEntity : public MovingEntity
{
public:
    PlayerEntity(const Vector2 & center, const Vector2 & size, const sf::Color & color, bigint elasticity, bigint friction, const Vector2 & velocity) :
        MovingEntity(center, size, color, elasticity, friction, velocity) {}

    virtual EntityType getType() { return EntityType_PlayerEntity; }

    virtual void doController(Game *game);
};

#endif // PLAYERENTITY_H
