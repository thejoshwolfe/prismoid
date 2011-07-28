#ifndef ENTITY_H
#define ENTITY_H

#include <SFML/Graphics.hpp>

class Entity
{
public:
    sf::Vector2f center;
    sf::Vector2f size;
    sf::Vector2f velocity;

    Entity(const sf::Vector2f & center, const sf::Vector2f & size);

    void render(sf::RenderTarget * render_target);
    sf::FloatRect getBoundingBox();
};

#endif // ENTITY_H
