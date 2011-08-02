#ifndef MOVINGENTITY_H
#define MOVINGENTITY_H

#include "Entity.h"

class MovingEntity : public Entity
{
public:
    MovingEntity(const sf::Vector2f & center, const sf::Vector2f & size, const sf::Color & color, const sf::Vector2f & velocity);

    virtual void render(sf::RenderTarget *render_target);
    virtual void getMotionBoundingPolygon(std::vector<sf::Vector2f> *polygon);

    sf::Vector2f * getVelocity() { return &velocity; }

    void calculateMotionBoundingPolygon();
    void detectCollision(Entity * other);
    void applyVelocity();

    void serialize(std::vector<byte>* buffer);
    static MovingEntity * deserialize(std::vector<byte>::const_iterator* buffer);

private:
    // important
    sf::Vector2f velocity;

    // temporary
    std::vector<bool> is_front_edge;
    std::vector<sf::Vector2f> motion_bounding_polygon;
};

#endif // MOVINGENTITY_H
