#ifndef ENTITY_H
#define ENTITY_H

#include "Util.h"

class Entity
{
public:
    virtual ~Entity() {}

    sf::Vector2f getCenter() { return center; }
    float getElasticity() { return elasticity; }
    float getFriction() { return friction; }

    virtual void render(sf::RenderTarget * render_target);

    virtual void getMotionBoundingPolygon(std::vector<sf::Vector2f> * polygon) = 0;

protected:
    sf::Vector2f center;
    sf::Vector2f size;
    sf::Color color;
    float elasticity;
    float friction;

    Entity(const sf::Vector2f & center, const sf::Vector2f & size, const sf::Color & color, float elasticity, float friction) :
        center(center), size(size), color(color), elasticity(elasticity), friction(friction) {}

    void getBoundingPolygon(std::vector<sf::Vector2f>* polygon);
};

#endif // ENTITY_H
