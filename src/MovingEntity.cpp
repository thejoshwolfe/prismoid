#include "MovingEntity.h"

MovingEntity::MovingEntity(const sf::Vector2f &center, const sf::Vector2f &size, const sf::Color &color, const sf::Vector2f &velocity) :
    Entity(center, size, color), velocity(velocity)
{
}

void MovingEntity::render(sf::RenderTarget *render_target)
{
    if (true) {
        // draw motion bounding polygon
        sf::Shape shape = sf::Shape();
        for (int i = 0; i < (int)motion_bounding_polygon.size(); i++)
            shape.AddPoint(motion_bounding_polygon[i], sf::Color::Green);
        render_target->Draw(shape);
    }

    Entity::render(render_target);
}

void MovingEntity::getMotionBoundingPolygon(std::vector<sf::Vector2f> *polygon)
{
    *polygon = motion_bounding_polygon;
}

void MovingEntity::calculateMotionBoundingPolygon()
{
    is_front_edge.clear();
    float velocity_magnitude = Util::magnitude(velocity);
    float velocity_angle = velocity_magnitude != 0 ? std::atan2(velocity.y, velocity.x) : 0;
    std::vector<sf::Vector2f> here;
    getBoundingPolygon(&here);
    // determine which edges face which ways
    for (int i = 0; i < (int)here.size(); i++) {
        sf::Vector2f point1 = here[i];
        sf::Vector2f point2 = here[(i + 1) % here.size()];
        sf::Vector2f edge = point2 - point1;
        float edge_angle = std::atan2(edge.y, edge.x);
        float relative_angle = Util::euclideanMod(velocity_angle - edge_angle, Util::two_pi);
        bool is_front = relative_angle > Util::pi;
        is_front_edge.push_back(is_front);
    }

    motion_bounding_polygon.clear();
    bool previously_front_edge = is_front_edge[is_front_edge.size() - 1];
    for (int i = 0; i < (int)here.size(); i++) {
        sf::Vector2f point = here[i];
        bool this_is_front_edge = is_front_edge[i];
        if (!this_is_front_edge) {
            if (previously_front_edge) {
                // insert backward pointing edge
                motion_bounding_polygon.push_back(point + velocity);
            }
            motion_bounding_polygon.push_back(point);
        } else {
            if (!previously_front_edge) {
                // insert forward pointing edge
                motion_bounding_polygon.push_back(point);
            }
            motion_bounding_polygon.push_back(point + velocity);
        }
        previously_front_edge = this_is_front_edge;
    }
}

void MovingEntity::detectCollision(Entity *other)
{
}

void MovingEntity::applyVelocity()
{
    center += velocity;
}

void MovingEntity::serialize(std::vector<byte> *buffer)
{
    Util::serialize(buffer, center);
    Util::serialize(buffer, size);
    Util::serialize(buffer, color);
    Util::serialize(buffer, velocity);
}

MovingEntity * MovingEntity::deserialize(std::vector<byte>::const_iterator* buffer)
{
    sf::Vector2f center = Util::deserialize<sf::Vector2f>(buffer);
    sf::Vector2f size = Util::deserialize<sf::Vector2f>(buffer);
    sf::Color color = Util::deserialize<sf::Color>(buffer);
    sf::Vector2f velocity = Util::deserialize<sf::Vector2f>(buffer);
    return new MovingEntity(center, size, color, velocity);
}
