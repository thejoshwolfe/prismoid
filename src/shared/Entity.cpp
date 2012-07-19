#include "Entity.h"

void Entity::render(Vector2 virtual_center, sf::RenderTarget *render_target)
{
    sf::Shape shape = sf::Shape();
    std::vector<Vector2> polygon;
    makeRectangle(&polygon, center, size);
    for (int i = 0; i < (int)polygon.size(); i++)
        shape.AddPoint(Util::toRenderPoint(virtual_center, polygon[i]), color);
    render_target->Draw(shape);
}

void Entity::makeRectangle(std::vector<Vector2> *polygon, const Vector2 &center, const Vector2 &size)
{
    // clockwise
    polygon->push_back(Vector2(center.x - size.x / 2, center.y - size.y / 2));
    polygon->push_back(Vector2(center.x + size.x / 2, center.y - size.y / 2));
    polygon->push_back(Vector2(center.x + size.x / 2, center.y + size.y / 2));
    polygon->push_back(Vector2(center.x - size.x / 2, center.y + size.y / 2));
}

