#ifndef PLAYERENTITY_H
#define PLAYERENTITY_H

#include "MovingEntity.h"

class PlayerEntity: public MovingEntity
{
public:
    PlayerEntity(const Vector2 & center, const Vector2 & size, const Vector2 & velocity) :
        MovingEntity(center, size, sf::Color::Blue, velocity), is_jumping(false)
    {
    }

    virtual EntityType getType()
    {
        return EntityType_PlayerEntity;
    }

    virtual void doController(Game *game);
private:
    bool is_jumping;
};

#endif
