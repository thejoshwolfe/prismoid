#include "Entity.h"

Entity::Entity(const sf::Vector2f & center, const sf::Vector2f & size, const sf::Vector2f & velocity, const sf::Color & color) :
    center(center), size(size), velocity(velocity), color(color)
{
}

void Entity::render(sf::RenderTarget *render_target)
{
    sf::Shape shape = sf::Shape();
    sf::FloatRect bounding_box = getBoundingBox();
    shape.AddPoint(bounding_box.Left, bounding_box.Top, color);
    shape.AddPoint(bounding_box.Right, bounding_box.Top, color);
    shape.AddPoint(bounding_box.Right, bounding_box.Bottom, color);
    shape.AddPoint(bounding_box.Left, bounding_box.Bottom, color);
    render_target->Draw(shape);
}

sf::FloatRect Entity::getBoundingBox()
{
    return sf::FloatRect(center.x - size.x / 2,
                         center.y - size.y / 2,
                         center.x + size.x / 2,
                         center.y + size.y / 2);
}

void Entity::serialize(std::vector<byte> *buffer)
{
    Util::serialize(buffer, center);
    Util::serialize(buffer, size);
    Util::serialize(buffer, velocity);
    Util::serialize(buffer, color);
}

Entity * Entity::deserialize(std::vector<byte>::const_iterator* buffer)
{
    sf::Vector2f center = Util::deserialize<sf::Vector2f>(buffer);
    sf::Vector2f size = Util::deserialize<sf::Vector2f>(buffer);
    sf::Vector2f velocity = Util::deserialize<sf::Vector2f>(buffer);
    sf::Color color = Util::deserialize<sf::Color>(buffer);
    return new Entity(center, size, velocity, color);
}
