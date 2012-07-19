#ifndef ENTITY_H
#define ENTITY_H

#include "Prismoid.h"

class Entity
{
public:
    virtual ~Entity() {}

    virtual const Vector2& getVelocity() const = 0;
    virtual void render(Vector2 virtual_center, sf::RenderTarget * render_target);

    // important
    Vector2 center;
    const Vector2 size;
    const sf::Color color;

    // inferred, cached.
    Prismoid bounding_prismoid;
    // temporary
    float frame_progress;

protected:
    Entity(const Vector2 & center, const Vector2 & size, const sf::Color & color) :
        center(center), size(size), color(color) {}

    static void makeRectangle(std::vector<Vector2>* polygon, const Vector2 &center, const Vector2 &size);
};

#endif // ENTITY_H
