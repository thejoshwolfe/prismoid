#ifndef ENTITY_H
#define ENTITY_H

#include "Prismoid.h"

class Entity
{
public:
    virtual ~Entity() {}

    virtual const Vector2& getVelocity() const = 0;
    virtual void render(Vector2 virtual_center, sf::RenderTarget * render_target);

    const bool is_moving_entity;

    // important
    Vector2 center;
    Vector2 size;
    sf::Color color;
    bigint elasticity;
    bigint friction;

    // inferred, cached.
    Prismoid bounding_prismoid;
    // temporary
    bigint frame_progress;

protected:
    Entity(bool is_moving_entity, const Vector2 & center, const Vector2 & size, const sf::Color & color, float elasticity, float friction) :
        is_moving_entity(is_moving_entity), center(center), size(size), color(color), elasticity(elasticity), friction(friction) {}

    static void makeRectangle(std::vector<Vector2>* polygon, const Vector2 &center, const Vector2 &size);
};

#endif // ENTITY_H
