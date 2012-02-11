#ifndef STATICENTITY_H
#define STATICENTITY_H

#include "Entity.h"

class StaticEntity : public Entity
{
public:
    StaticEntity(const Vector2 & center, const Vector2 & size, const sf::Color & color, float elasticity, float friction) :
        Entity(false, center, size, color, elasticity, friction)
    {
        bounding_prismoid.setZ(0, 1);
        std::vector<Vector2> here;
        makeRectangle(&here, center, size);
        for (int i = 0; i < (int)here.size(); i++)
            bounding_prismoid.addEdge(here[i], here[i]);
    }

    virtual const Vector2& getVelocity() const { return zero; }
private:
    static const Vector2 zero;
};

#endif // STATICENTITY_H
