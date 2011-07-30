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

    Entity(const sf::Vector2f & center, const sf::Vector2f & size, const sf::Vector2f & velocity, const sf::Color & color);

    void render(sf::RenderTarget * render_target);
    sf::FloatRect getBoundingBox();

    void serialize(std::vector<byte>* buffer);
    static Entity * deserialize(std::vector<byte>::const_iterator* buffer);
};

#endif // ENTITY_H
