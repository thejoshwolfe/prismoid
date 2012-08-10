#ifndef PLAYERENTITY_H
#define PLAYERENTITY_H

#include "MovingEntity.h"
#include "Renderable.h"

class PlayerEntity: public MovingEntity, public Renderable
{
public:
    PlayerEntity(const Vector2 & center, const Vector2 & size, const Vector2 & velocity) :
        MovingEntity(center, size, velocity), is_jumping(false)
    {
    }

    virtual EntityType getType()
    {
        return EntityType_PlayerEntity;
    }

    void doController(Game *game);
    void render(Vector2 virtual_center, sf::RenderTarget * render_target);
private:
    bool is_jumping;
};

#endif
