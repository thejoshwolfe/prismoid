#ifndef PLAYERENTITY_H
#define PLAYERENTITY_H

#include "MovingEntity.h"

class PlayerEntity : public MovingEntity
{
public:
    PlayerEntity(const sf::Vector2f & center, const sf::Vector2f & size, const sf::Color & color, float elasticity, float friction, const sf::Vector2f & velocity) :
        MovingEntity(center, size, color, elasticity, friction, velocity) {}

    virtual void doController(Game *game);
};

#endif // PLAYERENTITY_H
