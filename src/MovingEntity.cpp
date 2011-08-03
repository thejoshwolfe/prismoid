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
            shape.AddPoint(motion_bounding_polygon[i], collisions.empty() ? sf::Color::Green : sf::Color::Magenta);
        render_target->Draw(shape);
    }

    Entity::render(render_target);
}

void MovingEntity::getMotionBoundingPolygon(std::vector<sf::Vector2f> *polygon)
{
    *polygon = motion_bounding_polygon;
}

void MovingEntity::resetTemporaryState()
{
    is_front_edge.clear();
    motion_bounding_polygon.clear();
    collisions.clear();
}

void MovingEntity::calculateMotionBoundingPolygon()
{
    float velocity_angle = Util::angleOfVector(velocity);
    std::vector<sf::Vector2f> here;
    getBoundingPolygon(&here);
    // determine which edges face which ways
    calculateEdgesFacingAngle(here, &is_front_edge, velocity_angle);

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

void MovingEntity::calculateEdgesFacingAngle(const std::vector<sf::Vector2f> &polygon, std::vector<bool> *is_facing_edge, float facing_angle)
{
    for (int i = 0; i < (int)polygon.size(); i++) {
        sf::Vector2f point1 = polygon[i];
        sf::Vector2f point2 = polygon[(i + 1) % polygon.size()];
        sf::Vector2f edge = point2 - point1;
        float edge_angle = std::atan2(edge.y, edge.x);
        float relative_angle = Util::euclideanMod(facing_angle - edge_angle, Util::two_pi);
        bool is_front = relative_angle > Util::pi;
        is_facing_edge->push_back(is_front);
    }
}

void MovingEntity::detectCollision(Entity *other)
{
    std::vector<sf::Vector2f> here;
    getBoundingPolygon(&here);
    std::vector<sf::Vector2f> other_motion_bounding_polygon;
    other->getMotionBoundingPolygon(&other_motion_bounding_polygon);
    float velocity_magnitude = Util::magnitude(velocity);
    float velocity_angle = Util::angleOfVector(velocity);
    float opposite_velocity_angle = velocity_angle + Util::pi;
    std::vector<bool> is_other_back_edge;
    calculateEdgesFacingAngle(other_motion_bounding_polygon, &is_other_back_edge, opposite_velocity_angle);
    for (int i = 0; i < (int)here.size(); i++) {
        if (!(is_front_edge[i] || is_front_edge[Util::euclideanMod(i - 1, (int)is_front_edge.size())]))
            continue;
        // check this point against those edges
        sf::Vector2f this1 = here[i];
        for (int j = 0; j < (int)other_motion_bounding_polygon.size(); j++) {
            if (!is_other_back_edge[j])
                continue;
            sf::Vector2f other1 = other_motion_bounding_polygon[j];
            sf::Vector2f other2 = other_motion_bounding_polygon[(j + 1) % other_motion_bounding_polygon.size()];
            float distance_to_collision = distanceFromPointToSegment(this1, velocity, other1, other2);
            if (distance_to_collision <= velocity_magnitude) {
                // collision
                Util::insert(&collisions, distance_to_collision, Collision(other));
            }
        }
        if (is_front_edge[i]) {
            // check this edge against those points
            sf::Vector2f this2 = here[(i + 1) % here.size()];
            for (int j = 0; j < (int)other_motion_bounding_polygon.size(); j++) {
                if (!(is_other_back_edge[j] || is_other_back_edge[Util::euclideanMod(j - 1, (int)is_other_back_edge.size())]))
                    continue;
                sf::Vector2f other1 = other_motion_bounding_polygon[j];
                float distance_to_collision = distanceFromPointToSegment(other1, -velocity, this1, this2);
                if (distance_to_collision <= velocity_magnitude) {
                    // collision
                    Util::insert(&collisions, distance_to_collision, Collision(other));
                }
            }
        }
    }
}

float MovingEntity::distanceFromPointToSegment(const sf::Vector2f &point, const sf::Vector2f &direction, const sf::Vector2f &endpoint1, const sf::Vector2f &endpoint2)
{
    // http://objectmix.com/graphics/132701-ray-line-segment-intersection-2d.html#post460607
    sf::Vector2f segment_direction = endpoint2 - endpoint1;
    float denominator = Util::dot(Util::perp(segment_direction), direction);
    if (std::fabs(denominator) < std::numeric_limits<float>::epsilon())
        return std::numeric_limits<float>::infinity(); // parallel
    float t = Util::dot(Util::perp(direction), endpoint1 - point) / denominator;
    if (!(0 <= t && t <= 1))
        return std::numeric_limits<float>::infinity(); // miss
    float s = Util::dot(Util::perp(segment_direction), endpoint1 - point) / denominator;
    if (s < 0)
        return std::numeric_limits<float>::infinity(); // behind
    return s * Util::magnitude(direction);
}

void MovingEntity::resolveCollisionsAndApplyVelocity()
{
    if (collisions.empty()) {
        center += velocity;
        return;
    }
    std::multimap<float, Collision>::iterator iterator = collisions.begin();
    float closest_collision = iterator->first;
    center += closest_collision * Util::normalized(velocity);
    velocity = sf::Vector2f(0, 0);
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
