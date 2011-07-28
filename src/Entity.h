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

    sf::Drawable * toDrawable();
    sf::FloatRect getBoundingBox();
};

#endif // ENTITY_H
