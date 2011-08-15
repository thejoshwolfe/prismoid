#ifndef ENTITY_H
#define ENTITY_H

#include "Util.h"

class Entity
{
public:
    virtual ~Entity() {}

    virtual const sf::Vector2f& getVelocity() const = 0;
    virtual void render(sf::RenderTarget * render_target);

    const bool is_moving_entity;

    // important
    sf::Vector2f center;
    sf::Vector2f size;
    sf::Color color;
    float elasticity;
    float friction;

    // inferred, cached.
    Prismoid bounding_prismoid;
    // temporary
    float frame_progress;

protected:
    Entity(bool is_moving_entity, const sf::Vector2f & center, const sf::Vector2f & size, const sf::Color & color, float elasticity, float friction) :
        is_moving_entity(is_moving_entity), center(center), size(size), color(color), elasticity(elasticity), friction(friction) {}

    static void makeRectangle(std::vector<sf::Vector2f>* polygon, const sf::Vector2f &center, const sf::Vector2f &size);
};

#endif // ENTITY_H
