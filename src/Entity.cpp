#include "Entity.h"

void Entity::render(sf::RenderTarget *render_target)
{
    sf::Shape shape = sf::Shape();
    std::vector<sf::Vector2f> polygon;
    getBoundingPolygon(&polygon);
    for (int i = 0; i < (int)polygon.size(); i++)
        shape.AddPoint(polygon[i], color);
    render_target->Draw(shape);
}

void Entity::getBoundingPolygon(std::vector<sf::Vector2f> *polygon)
{
    // clockwise
    polygon->push_back(sf::Vector2f(center.x - size.x / 2, center.y - size.y / 2));
    polygon->push_back(sf::Vector2f(center.x + size.x / 2, center.y - size.y / 2));
    polygon->push_back(sf::Vector2f(center.x + size.x / 2, center.y + size.y / 2));
    polygon->push_back(sf::Vector2f(center.x - size.x / 2, center.y + size.y / 2));
}

