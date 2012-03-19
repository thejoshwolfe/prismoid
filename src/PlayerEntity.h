#ifndef PLAYERENTITY_H
#define PLAYERENTITY_H

#include "MovingEntity.h"

class PlayerEntity : public MovingEntity
{
public:
    PlayerEntity(const Vector2 & center) :
        MovingEntity(center, Vector2(30, 84), sf::Color::Blue),
        is_jumping(false) {}

    virtual EntityType getType() { return EntityType_PlayerEntity; }

    virtual void doController(Game *game);
private:
    bool is_jumping;
};

#endif
