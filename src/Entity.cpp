#include "Entity.h"

void Entity::render(sf::RenderTarget *render_target)
{
    sf::Shape shape = sf::Shape();
    std::vector<sf::Vector2f> polygon;
    makeRectangle(&polygon, center, size);
    for (int i = 0; i < (int)polygon.size(); i++)
        shape.AddPoint(polygon[i], color);
    render_target->Draw(shape);
}

void Entity::makeRectangle(std::vector<sf::Vector2f> *polygon, const sf::Vector2f &center, const sf::Vector2f &size)
{
    // clockwise
    polygon->push_back(sf::Vector2f(center.x - size.x / 2, center.y - size.y / 2));
    polygon->push_back(sf::Vector2f(center.x + size.x / 2, center.y - size.y / 2));
    polygon->push_back(sf::Vector2f(center.x + size.x / 2, center.y + size.y / 2));
    polygon->push_back(sf::Vector2f(center.x - size.x / 2, center.y + size.y / 2));
}

