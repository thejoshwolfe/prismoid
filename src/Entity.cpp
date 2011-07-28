#include "Entity.h"

Entity::Entity(const sf::Vector2f & center, const sf::Vector2f & size) :
    center(center), size(size), velocity(0, 0)
{
    toDrawable();
}

sf::Drawable * Entity::toDrawable()
{
    sf::Shape * shape = new sf::Shape();
    sf::Color color = sf::Color::Blue;
    sf::FloatRect bounding_box = getBoundingBox();
    shape->AddPoint(bounding_box.Left, bounding_box.Top, color);
    shape->AddPoint(bounding_box.Right, bounding_box.Top, color);
    shape->AddPoint(bounding_box.Right, bounding_box.Bottom, color);
    shape->AddPoint(bounding_box.Left, bounding_box.Bottom, color);
    return shape;
}

sf::FloatRect Entity::getBoundingBox()
{
    return sf::FloatRect(center.x - size.x / 2,
                         center.y - size.y / 2,
                         center.x + size.x / 2,
                         center.y + size.y / 2);
}
