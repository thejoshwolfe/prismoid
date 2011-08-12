#include "MovingEntity.h"

#include "Game.h"
#include "PlayerEntity.h"

void MovingEntity::render(sf::RenderTarget *render_target)
{
    if (true) {
        // draw motion bounding polygon
        sf::Shape shape;
        for (int i = 0; i < bounding_prismoid.size(); i++) {
            Edge edge;
            bounding_prismoid.getEdge(i, &edge);
            shape.AddPoint(sf::Vector2f(edge.points[0].x, edge.points[0].y), sf::Color::Green);
        }
        render_target->Draw(shape);
    }

    Entity::render(render_target);
}

void MovingEntity::getBoundingPrismoid(Prismoid *prismoid)
{
    *prismoid = bounding_prismoid;
}

void MovingEntity::resetForNextFrame()
{
    frame_progress = 0;
}

void MovingEntity::doController(Game *)
{
    velocity.y += 0.4f;
}

void MovingEntity::calculateBoundingPrismoid()
{
    sf::Vector2f remaining_velocity = (1 - frame_progress) * velocity;
    std::vector<sf::Vector2f> here;
    getBoundingPolygon(&here);

    bounding_prismoid.clearEdges();
    bounding_prismoid.setZ(frame_progress, 1);
    for (int i = 0; i < (int)here.size(); i++) {
        sf::Vector2f point = here[i];
        bounding_prismoid.addEdge(point, point + remaining_velocity);
    }
}

void MovingEntity::detectCollision(Entity *other)
{
    Prismoid other_prismoid;
    other->getBoundingPrismoid(&other_prismoid);
    for (int i = 0; i < other_prismoid.size(); i++) {
        Edge other_edge1, other_edge2;
        other_prismoid.getFace(i, &other_edge1, &other_edge2);
        for (int j = 0; j < bounding_prismoid.size(); j++) {
            Edge this_edge;
            bounding_prismoid.getEdge(j, &this_edge);
            sf::Vector3f collision_point;
            bool is_collision = Util::getEdgeIntersectionWithQuadrilateral(this_edge, other_edge1, other_edge2, &collision_point);
            if (!is_collision)
                continue;
            Util::push(&collisions, collision_point.z, Collision(other, other_prismoid.getNormal(i)));
        }
    }
    for (int i = 0; i < bounding_prismoid.size(); i++) {
        Edge this_edge1, this_edge2;
        bounding_prismoid.getFace(i, &this_edge1, &this_edge2);
        for (int j = 0; j < other_prismoid.size(); j++) {
            Edge other_edge;
            other_prismoid.getEdge(j, &other_edge);
            sf::Vector3f collision_point;
            bool is_collision = Util::getEdgeIntersectionWithQuadrilateral(other_edge, this_edge1, this_edge2, &collision_point);
            if (!is_collision)
                continue;
            Util::push(&collisions, collision_point.z, Collision(other, -bounding_prismoid.getNormal(i)));
        }
    }
}

bool MovingEntity::moveToFirstCollision()
{
    if (collisions.empty()) {
        center += (1 - frame_progress) * velocity;
        return false;
    }

    float soonest_collision_time = collisions.top().key;
    Entity * other = collisions.top().value.entity;
    // the biggest dot product means the steepest collision angle
    sf::Vector2f normal_with_biggest_dot_product = collisions.top().value.normal;
    float biggest_dot_product = Util::dot(velocity, normal_with_biggest_dot_product);
    collisions.pop();
    for (; !collisions.empty(); collisions.pop()) {
        // accept any collision at pretty much the same place as the first one
        float collision_time = collisions.top().key;
        if (collision_time - soonest_collision_time > std::numeric_limits<float>::epsilon())
            break; // too far away
        sf::Vector2f normal = collisions.top().value.normal;
        float dot_product = Util::dot(velocity, normal);
        if (dot_product > biggest_dot_product) {
            // found a steeper collision
            biggest_dot_product = dot_product;
            normal_with_biggest_dot_product = normal;
            other = collisions.top().value.entity;
        }
    }

    // snap to the collision point
    center += (soonest_collision_time - frame_progress) * velocity;
    frame_progress = soonest_collision_time;

    // bounce
    sf::Vector2f normal = Util::normalized(normal_with_biggest_dot_product);
    float elasticity = this->elasticity * other->getElasticity();
    sf::Vector2f normal_component = Util::dot(velocity, normal) * normal;
    sf::Vector2f tangent_component = velocity - normal_component;
    sf::Vector2f normal_force = -(1 + elasticity) * normal_component;
    float friction_coefficient = this->friction * other->getFriction();
    float friction_magnitude = Util::min(friction_coefficient * Util::magnitude(normal_force), Util::magnitude(tangent_component));
    sf::Vector2f friction_force = -friction_magnitude * Util::normalized(tangent_component);
    velocity += normal_force + friction_force;

    return true;
}

void MovingEntity::serialize(std::vector<byte> *buffer)
{
    Util::serialize(buffer, (int32)getType());
    Util::serialize(buffer, center);
    Util::serialize(buffer, size);
    Util::serialize(buffer, color);
    Util::serialize(buffer, elasticity);
    Util::serialize(buffer, friction);
    Util::serialize(buffer, velocity);
}

MovingEntity * MovingEntity::deserialize(std::vector<byte>::const_iterator* buffer)
{
    EntityType type = (EntityType)Util::deserialize<int32>(buffer);
    sf::Vector2f center = Util::deserialize<sf::Vector2f>(buffer);
    sf::Vector2f size = Util::deserialize<sf::Vector2f>(buffer);
    sf::Color color = Util::deserialize<sf::Color>(buffer);
    float elasticity = Util::deserialize<float>(buffer);
    float friction = Util::deserialize<float>(buffer);
    sf::Vector2f velocity = Util::deserialize<sf::Vector2f>(buffer);
    switch (type) {
        case EntityType_MovingEntity:
            return new MovingEntity(center, size, color, elasticity, friction, velocity);
        case EntityType_PlayerEntity:
            return new PlayerEntity(center, size, color, elasticity, friction, velocity);
    }
    // TODO: panic properly
    throw 1;
}
