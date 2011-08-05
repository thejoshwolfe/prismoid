#ifndef STATICENTITY_H
#define STATICENTITY_H

#include "Entity.h"

class StaticEntity : public Entity
{
public:
    StaticEntity(const sf::Vector2f & center, const sf::Vector2f & size, const sf::Color & color, float elasticity, float friction) :
        Entity(center, size, color, elasticity, friction) {}

public:
    virtual void getMotionBoundingPolygon(std::vector<sf::Vector2f> * polygon);
};

#endif // STATICENTITY_H
