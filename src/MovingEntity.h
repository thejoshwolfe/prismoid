#ifndef MOVINGENTITY_H
#define MOVINGENTITY_H

#include "Entity.h"

class Game;

class MovingEntity : public Entity
{
public:
    MovingEntity(const sf::Vector2f & center, const sf::Vector2f & size, const sf::Color & color, float elasticity, const sf::Vector2f & velocity);

    virtual void render(sf::RenderTarget *render_target);
    virtual void getMotionBoundingPolygon(std::vector<sf::Vector2f> *polygon);

    virtual void doController(Game * game);

    void resetForNextFrame();
    void calculateMotionBoundingPolygon();
    void detectCollision(Entity * other);
    /** returns whether it's got more distance to go in this frame */
    bool resolveCollisionsAndApplyVelocity();

    void serialize(std::vector<byte>* buffer);
    static MovingEntity * deserialize(std::vector<byte>::const_iterator* buffer);

private:
    struct Collision {
        Entity * entity;
        sf::Vector2f normal;
        Collision(Entity * entity, const sf::Vector2f &normal)
            : entity(entity), normal(normal) {}
    };

    // important
    sf::Vector2f velocity;

    // temporary
    std::vector<bool> is_front_edge;
    std::vector<sf::Vector2f> motion_bounding_polygon;
    std::multimap<float, Collision> collisions;
    float remaining_velocity_percent;

    static void calculateEdgesFacingAngle(const std::vector<sf::Vector2f> &polygon, std::vector<bool> * is_facing_edge, float facing_angle);

    static float distanceFromPointToSegment(const sf::Vector2f &point, const sf::Vector2f &direction, const sf::Vector2f &segment_start, const sf::Vector2f &segment_direction);
};

#endif // MOVINGENTITY_H
