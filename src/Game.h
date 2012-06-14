#ifndef GAME_H
#define GAME_H

#include "MovingEntity.h"
#include "StaticEntity.h"
#include "TiledTmx.h"

#include <tr1/memory>

class Game
{
public:
    Game(std::string filename);

    void doFrame(const sf::Input * input);
    void render(sf::RenderTarget * render_target);

    const sf::Input * getInput() { return input; }

private:
    // important
    std::vector<MovingEntity *> moving_entities;
    sf::Image tileset_image;
    std::map<IntRectWithCompareOperator, sf::Image*> tileset_images;
    int tile_size;
    int layer_width;
    int layer_height;
    StaticEntity ** physics_layer;

    // temporary
    const sf::Input * input;

    struct Collision {
        bool valid;
        MovingEntity * entity;
        Entity * other;
        float time;
        Vector2 normal;
        Collision(MovingEntity * entity, Entity * other, float time, const Vector2 &normal) :
            valid(true), entity(entity), other(other), time(time), normal(normal) {}
    };
    std::multimap<Entity *, std::tr1::shared_ptr<Collision> > collisions_by_entity;
    std::priority_queue<Util::KeyAndValue<float, std::tr1::shared_ptr<Collision> > > collisions_by_time;

    void detectCollisions(MovingEntity * entity);
    bool detectCollision(MovingEntity * entity, Entity * other);
    static bool getEdgeIntersectionWithQuadrilateral(const Edge &edge, const Edge &plane_edge1, const Edge &plane_edge2, Vector3 *output_intersection_point);
    bool maybeAddCollision(float time, MovingEntity * entity, Entity * other, const Vector2 &normal, const Vector2 &adjacent_edge1, const Vector2 &adjacent_edge2);
    void doCollision(float time, const std::vector<std::tr1::shared_ptr<Collision> > &collisions);
    void invalidateCollisions(MovingEntity *entity);

    Rectangle getBoundingRectangle(const Prismoid &prismoid);
    void getStaticEntities(std::vector<StaticEntity *> &static_entities, const Rectangle &bounding_rectangle);
};

#endif // GAME_H
