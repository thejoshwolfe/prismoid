#include "MovingEntity.h"

#include "Game.h"

MovingEntity::MovingEntity(const sf::Vector2f &center, const sf::Vector2f &size, const sf::Color &color, float elasticity, const sf::Vector2f &velocity) :
    Entity(center, size, color, elasticity), velocity(velocity)
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

void MovingEntity::resetForNextFrame()
{
    // TODO: we can do this whenever we return true from the other thing
    remaining_velocity_percent = 1;
}

void MovingEntity::doController(Game *game)
{
    const sf::Input * input = game->getInput();
    const float move_acceleration = 0.5f;
    if (input->IsKeyDown(sf::Key::W)) velocity.y -= move_acceleration;
    if (input->IsKeyDown(sf::Key::A)) velocity.x -= move_acceleration;
    if (input->IsKeyDown(sf::Key::S)) velocity.y += move_acceleration;
    if (input->IsKeyDown(sf::Key::D)) velocity.x += move_acceleration;
    velocity.y += 0.4f;
}

void MovingEntity::calculateMotionBoundingPolygon()
{
    sf::Vector2f remaining_velocity = remaining_velocity_percent * velocity;
    float velocity_angle = Util::angleOfVector(remaining_velocity);
    std::vector<sf::Vector2f> here;
    getBoundingPolygon(&here);
    // determine which edges face which ways
    is_front_edge.clear();
    calculateEdgesFacingAngle(here, &is_front_edge, velocity_angle);

    motion_bounding_polygon.clear();
    bool previously_front_edge = is_front_edge[is_front_edge.size() - 1];
    for (int i = 0; i < (int)here.size(); i++) {
        sf::Vector2f point = here[i];
        bool this_is_front_edge = is_front_edge[i];
        if (!this_is_front_edge) {
            if (previously_front_edge) {
                // insert backward pointing edge
                motion_bounding_polygon.push_back(point + remaining_velocity);
            }
            motion_bounding_polygon.push_back(point);
        } else {
            if (!previously_front_edge) {
                // insert forward pointing edge
                motion_bounding_polygon.push_back(point);
            }
            motion_bounding_polygon.push_back(point + remaining_velocity);
        }
        previously_front_edge = this_is_front_edge;
    }

    // ugly coding, but putting this here will work.
    collisions.clear();
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
    sf::Vector2f remaining_velocity = remaining_velocity_percent * velocity;
    float remaining_velocity_magnitude = Util::magnitude(remaining_velocity);
    float velocity_angle = Util::angleOfVector(remaining_velocity);
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
            sf::Vector2f segment_direction = other2 - other1;
            float distance_to_collision = distanceFromPointToSegment(this1, remaining_velocity, other1, segment_direction);
            if (distance_to_collision <= remaining_velocity_magnitude) {
                // collision
                Util::insert(&collisions, distance_to_collision, Collision(other, Util::perp(Util::normalized(segment_direction))));
            }
        }
        if (is_front_edge[i]) {
            // check this edge against those points
            sf::Vector2f this2 = here[(i + 1) % here.size()];
            for (int j = 0; j < (int)other_motion_bounding_polygon.size(); j++) {
                if (!(is_other_back_edge[j] || is_other_back_edge[Util::euclideanMod(j - 1, (int)is_other_back_edge.size())]))
                    continue;
                sf::Vector2f other1 = other_motion_bounding_polygon[j];
                sf::Vector2f segment_direction = this2 - this1;
                float distance_to_collision = distanceFromPointToSegment(other1, -remaining_velocity, this1, segment_direction);
                if (distance_to_collision <= remaining_velocity_magnitude) {
                    // collision
                    Util::insert(&collisions, distance_to_collision, Collision(other, -Util::perp(Util::normalized(segment_direction))));
                }
            }
        }
    }
}

float MovingEntity::distanceFromPointToSegment(const sf::Vector2f &point, const sf::Vector2f &direction, const sf::Vector2f &segment_start, const sf::Vector2f &segment_direction)
{
    // http://objectmix.com/graphics/132701-ray-line-segment-intersection-2d.html#post460607
    float denominator = Util::dot(Util::perp(segment_direction), direction);
    if (std::fabs(denominator) < std::numeric_limits<float>::epsilon())
        return std::numeric_limits<float>::infinity(); // parallel
    float t = Util::dot(Util::perp(direction), segment_start - point) / denominator;
    if (!(0 <= t && t <= 1))
        return std::numeric_limits<float>::infinity(); // miss
    float s = Util::dot(Util::perp(segment_direction), segment_start - point) / denominator;
    if (s < 0)
        return std::numeric_limits<float>::infinity(); // behind
    return s * Util::magnitude(direction);
}

bool MovingEntity::resolveCollisionsAndApplyVelocity()
{
    if (collisions.empty()) {
        center += remaining_velocity_percent * velocity;
        return false;
    }
    sf::Vector2f normalized_velocity = Util::normalized(velocity);
    std::multimap<float, Collision>::iterator iterator = collisions.begin();
    float closest_collision = iterator->first;
    Entity * other = iterator->second.entity;
    // the biggest dot product means the steepest collision angle
    sf::Vector2f normal_with_biggest_dot_product = iterator->second.normal;
    float biggest_dot_product = Util::dot(normalized_velocity, normal_with_biggest_dot_product);
    iterator++;
    for (; iterator != collisions.end(); iterator++) {
        // accept any collision at pretty much the same place as the first one
        float distance_to_collision = iterator->first;
        if (distance_to_collision - closest_collision > std::numeric_limits<float>::epsilon())
            break; // too far away
        sf::Vector2f normal = iterator->second.normal;
        float dot_product = Util::dot(normalized_velocity, normal);
        if (dot_product > biggest_dot_product) {
            // found a steeper collision
            biggest_dot_product = dot_product;
            normal_with_biggest_dot_product = normal;
            other = iterator->second.entity;
        }
    }

    // snap to the collision point
    center += closest_collision * normalized_velocity;

    // how much velocity do we have left to use
    float velocity_magnitude = Util::magnitude(velocity);
    if (velocity_magnitude != 0)
        remaining_velocity_percent -= closest_collision / velocity_magnitude;
    else
        remaining_velocity_percent = 0;

    // bounce
    float elasticity = this->elasticity * other->getElasticity();
    velocity = velocity - (1 + elasticity) * Util::dot(velocity, normal_with_biggest_dot_product) * normal_with_biggest_dot_product;

    return remaining_velocity_percent > 0;
}

void MovingEntity::serialize(std::vector<byte> *buffer)
{
    Util::serialize(buffer, center);
    Util::serialize(buffer, size);
    Util::serialize(buffer, color);
    Util::serialize(buffer, elasticity);
    Util::serialize(buffer, velocity);
}

MovingEntity * MovingEntity::deserialize(std::vector<byte>::const_iterator* buffer)
{
    sf::Vector2f center = Util::deserialize<sf::Vector2f>(buffer);
    sf::Vector2f size = Util::deserialize<sf::Vector2f>(buffer);
    sf::Color color = Util::deserialize<sf::Color>(buffer);
    float elasticity = Util::deserialize<float>(buffer);
    sf::Vector2f velocity = Util::deserialize<sf::Vector2f>(buffer);
    return new MovingEntity(center, size, color, elasticity, velocity);
}
