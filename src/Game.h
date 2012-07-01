#ifndef GAME_H
#define GAME_H

#include "MovingEntity.h"
#include "StaticEntity.h"
#include "TiledTmx.h"

#include <tr1/memory>

struct Collision {
    bool valid;
    bool is_actual_collision;

    // so we know which one is the moving one.
    MovingEntity * moving_entity;
    // one of these two will be the same as the one above
    Entity * vertex_entity;
    Entity * edge_entity;
    int vertex_index;
    int edge_index;

    float time;
    /** real collision */
    Collision(MovingEntity * moving_entity, Entity * vertex_entity, Entity * edge_entity, int vertex_index, int edge_index, float time) :
        valid(true), is_actual_collision(true), moving_entity(moving_entity), vertex_entity(vertex_entity), edge_entity(edge_entity), vertex_index(vertex_index), edge_index(edge_index), time(time) {}
    /** fake collision */
    explicit Collision(MovingEntity * moving_entity) :
        valid(true), is_actual_collision(false), moving_entity(moving_entity), time(1) {}
    Vector2 getNormal() const
    {
        Vector2 result = edge_entity->bounding_prismoid.getNormal(edge_index);
        if (edge_entity == moving_entity)
            result = -result;
        return result;
    }
};

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
    std::map<sf::IntRect, sf::Image*> tileset_images;
    int tile_size;
    int layer_width;
    int layer_height;
    StaticEntity ** physics_layer;

    // temporary
    const sf::Input * input;

    std::multimap<MovingEntity *, std::tr1::shared_ptr<Collision> > collisions_by_entity;
    std::priority_queue<Util::KeyAndValue<float, std::tr1::shared_ptr<Collision> > > collisions_by_time;

    void detectCollisions(MovingEntity * entity);
    bool detectCollision(MovingEntity * entity, Entity * other);
    static bool getEdgeIntersectionWithQuadrilateral(const Edge &edge, const Edge &plane_edge1, const Edge &plane_edge2, Vector3 *output_intersection_point);
    bool maybeAddCollision(std::tr1::shared_ptr<Collision> collision);
    void addCollision(std::tr1::shared_ptr<Collision> collision);
    void doCollisions(float time, const std::vector<std::tr1::shared_ptr<Collision> > &collisions);
    void invalidateCollisions(MovingEntity *entity);

    Rectangle getBoundingRectangle(const Prismoid &prismoid);
    void getStaticEntities(std::vector<StaticEntity *> &static_entities, const Rectangle &bounding_rectangle);
};

#endif // GAME_H
