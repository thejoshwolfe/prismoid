#ifndef ENTITY_H
#define ENTITY_H

#include "Prismoid.h"

class Entity
{
public:
    virtual ~Entity() {}

    virtual const Vector2& getVelocity() const = 0;

    // important
    Vector2 center;
    const Vector2 size;

    // inferred, cached.
    Prismoid bounding_prismoid;
    // temporary
    float frame_progress;

protected:
    Entity(const Vector2 & center, const Vector2 & size) :
        center(center), size(size) {}
};

#endif // ENTITY_H
