#ifndef STATICENTITY_H
#define STATICENTITY_H

#include "Entity.h"

class StaticEntity : public Entity
{
public:
    StaticEntity(const Vector2 & center, const Vector2 & size) :
        Entity(center, size)
    {
    }

    virtual const Vector2& getVelocity() const { return zero; }
private:
    static const Vector2 zero;
};

#endif
