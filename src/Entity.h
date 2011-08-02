#ifndef ENTITY_H
#define ENTITY_H

#include "Util.h"

class Entity
{
public:
    sf::Vector2f center;
    sf::Vector2f size;
    sf::Vector2f velocity;
    sf::Color color;
    std::vector<sf::Vector2f> motion_bounding_polygon;
    std::vector<bool> is_front_edge;

    Entity(const sf::Vector2f & center, const sf::Vector2f & size, const sf::Vector2f & velocity, const sf::Color & color);

    void render(sf::RenderTarget * render_target);
    void calculateMotionBoundingPolygon();
    void getBoundingPolygon(std::vector<sf::Vector2f>* polygon);

    void serialize(std::vector<byte>* buffer);
    static Entity * deserialize(std::vector<byte>::const_iterator* buffer);
};

#endif // ENTITY_H
