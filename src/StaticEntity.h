#ifndef STATICENTITY_H
#define STATICENTITY_H

#include "Entity.h"

class StaticEntity : public Entity
{
public:
    StaticEntity(const sf::Vector2f & center, const sf::Vector2f & size, const sf::Color & color, float elasticity, float friction) :
        Entity(false, center, size, color, elasticity, friction)
    {
        bounding_prismoid.setZ(0, 1);
        std::vector<sf::Vector2f> here;
        makeRectangle(&here, center, size);
        for (int i = 0; i < (int)here.size(); i++)
            bounding_prismoid.addEdge(here[i], here[i]);
    }

    virtual const sf::Vector2f& getVelocity() const { return zero; }
private:
    static const sf::Vector2f zero;
};

#endif // STATICENTITY_H
