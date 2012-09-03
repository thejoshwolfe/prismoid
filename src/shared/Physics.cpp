#include "Physics.h"

// debugging
#include <iostream>
#include "debugging_helpers.h"

void Physics::doFrame()
{
    // determine desired motion
    for (int i = 0; i < (int)moving_entities.size(); i++)
        moving_entities[i]->calculateBoundingPrismoid();
    // detect collisions
    for (int i = 0; i < (int)moving_entities.size(); i++)
        detectCollisions(moving_entities[i]);

    // move everything in the right order
    while (!collisions_by_time.empty()) {
        float time = collisions_by_time.top().key;
        // resolve all the collisions at the first interesting instant simultaneously.
        std::vector<std::tr1::shared_ptr<Collision> > collisions;
        while (!collisions_by_time.empty() && collisions_by_time.top().key == time) {
            std::tr1::shared_ptr<Collision> collision = collisions_by_time.top().value;
            if (collision->valid)
                collisions.push_back(collision);
            collisions_by_time.pop();
        }
        if (!collisions.empty())
            doCollisions(time, collisions);
    }
    collisions_by_entity.clear();

    // reset frame progress
    for (int i = 0; i < (int)moving_entities.size(); i++)
        moving_entities[i]->frame_progress = 0;
}

void Physics::detectCollisions(MovingEntity * entity)
{
    bool ever_added = false;

    // only check static entities in the tiles we'll be intersecting.
    Rectangle bounding_rectangle = getBoundingRectangle(entity->bounding_prismoid);
    std::vector<StaticEntity *> static_entities;
    static_entity_provider->getStaticEntities(static_entities, bounding_rectangle);
    for (int j = 0; j < (int)static_entities.size(); j++)
        ever_added |= detectCollision(entity, static_entities[j]);

    // O(n^2) for collision detection with other moving entities.
    for (int j = 0; j < (int)moving_entities.size(); j++) {
        MovingEntity * other_entity = moving_entities[j];
        if (entity != other_entity)
            ever_added |= detectCollision(entity, other_entity);
    }
    if (!ever_added) {
        // add a fake collision so that the entity will be updated at all
        addCollision(std::tr1::shared_ptr<Collision>(new Collision(entity)));
    }
}

bool Physics::detectCollision(MovingEntity *moving_entity, Entity *other)
{
    bool ever_added = false;
    Entity* entities[] = { moving_entity, other, };
    for (int entity_index = 0; entity_index < 2; entity_index++) {
        Entity * edge_entity = entities[entity_index];
        Entity * face_entity = entities[1 - entity_index];
        for (int face_index = 0; face_index < face_entity->bounding_prismoid.size(); face_index++) {
            Edge face_edge1, face_edge2;
            face_entity->bounding_prismoid.getFace(face_index, &face_edge1, &face_edge2);
            for (int edge_index = 0; edge_index < edge_entity->bounding_prismoid.size(); edge_index++) {
                Edge edge;
                edge_entity->bounding_prismoid.getEdge(edge_index, &edge);
                Vector3 collision_point;
                bool is_collision = getEdgeIntersectionWithQuadrilateral(edge, face_edge1, face_edge2, &collision_point);
                if (!is_collision)
                    continue;
                std::tr1::shared_ptr<Collision> collision(new Collision(moving_entity, edge_entity, face_entity, edge_index, face_index, collision_point.z));
                std::cout << "Adding collision: " << s(collision) << std::endl;
                addCollision(collision);
                ever_added = true;
            }
        }
    }
    return ever_added;
}

bool Physics::getEdgeIntersectionWithQuadrilateral(const Edge &edge, const Edge &plane_edge1, const Edge &plane_edge2, Vector3 *output_intersection_point)
{
    // http://en.wikipedia.org/wiki/Line-plane_intersection
    Vector3 edge_point = edge.points[0];
    Vector3 edge_vector = edge.points[1] - edge_point;
    Vector3 plane_point = plane_edge1.points[0];
    Vector3 plane_vector = Util::cross(plane_edge1.points[1] - plane_point, plane_edge2.points[0] - plane_point);
    float numerator = Util::dot(plane_point - edge_point, plane_vector);
    Vector3 intersection_point;
    if (numerator == 0) {
        // intersects immediately
        intersection_point = edge_point;
    } else {
        float denominator = Util::dot(edge_vector, plane_vector);
        if (denominator == 0) {
            // parallel and not intersecting
            return false;
        }
        float percent_to_intersection = numerator / denominator;
        // is it in front of us, and can we reach it?
        if (!(0 <= percent_to_intersection && percent_to_intersection <= 1))
            return false;
        intersection_point = edge_point + percent_to_intersection * edge_vector;
    }

    // check the bounds of the face
    Vector3 plane_points[] = { plane_edge1.points[0], plane_edge1.points[1], plane_edge2.points[1], plane_edge2.points[0], };
    const int plane_points_count = sizeof(plane_points) / sizeof(Vector3);
    for (int i = 0; i < plane_points_count; i++) {
        Vector3 edge1 = plane_points[(i + 3) % plane_points_count] - plane_points[i];
        Vector3 edge2 = plane_points[(i + 1) % plane_points_count] - plane_points[i];
        Vector3 inward_direction = Util::cross(edge2, Util::cross(edge1, edge2));
        bool is_inside = Util::dot(inward_direction, intersection_point - plane_points[i]) >= 0;
        if (!is_inside)
            return false; // miss
    }

    // hit
    *output_intersection_point = intersection_point;
    return true;
}

bool Physics::maybeAddCollision(std::tr1::shared_ptr<Collision> collision)
{
    // don't bother considering collisions between vertexes and edges that physically can't ever contact each other.
    // this is kinda like backface culling.
    Vector2 adjacent_edge1, adjacent_edge2;
    collision->vertex_entity->bounding_prismoid.getAdjacentCapEdgeVectors(collision->vertex_index, &adjacent_edge1, &adjacent_edge2);
    Vector2 bounding_normal1 = -Util::normalized(Util::perp(adjacent_edge1));
    Vector2 bounding_normal2 = Util::normalized(Util::perp(adjacent_edge2));
    float vertex_angle = Util::dot(bounding_normal1, bounding_normal2);
    Vector2 edge_normal = Util::normalized(collision->edge_entity->bounding_prismoid.getNormal(collision->edge_index));
    if (Util::dot(bounding_normal1, edge_normal) < vertex_angle)
        return false;
    if (Util::dot(bounding_normal2, edge_normal) < vertex_angle)
        return false;
    addCollision(collision);
    return true;
}

void Physics::addCollision(std::tr1::shared_ptr<Collision> collision)
{
    Util::push(&collisions_by_time, collision->time, collision);
    Util::insert(&collisions_by_entity, collision->moving_entity, collision);
}

void Physics::doCollisions(float time, const std::vector<std::tr1::shared_ptr<Collision> > &collisions)
{
    // TODO: this block assumes only 1 moving entity.
    {
        // some things are going to be identical regardless of the number of collisions
        std::tr1::shared_ptr<Collision> collision = collisions[0];
        MovingEntity * moving_entity = collision->moving_entity;

        // snap to the collision point
        moving_entity->center += Util::scaleVector(time - moving_entity->frame_progress, moving_entity->velocity);
        moving_entity->frame_progress = time;

        if (!collision->is_actual_collision)
            return;

        // recalculate collisions for this entity
        invalidateCollisions(moving_entity);
    }

    // deal with each pair of entities individually
    std::set<std::pair<MovingEntity *, Entity *> > entities;
    std::multimap<std::pair<MovingEntity *, Entity *>, std::tr1::shared_ptr<Collision> > entities_to_collisions;

    for (int i = 0; i < (int)collisions.size(); i++) {
        std::tr1::shared_ptr<Collision> collision = collisions[i];
        std::pair<MovingEntity *, Entity *> pair(collision->moving_entity, collision->moving_entity == collision->vertex_entity ? collision->edge_entity : collision->vertex_entity);
        entities.insert(pair);
        Util::insert(&entities_to_collisions, pair, collision);
    }
    for (std::set<std::pair<MovingEntity *, Entity *> >::iterator entities_iterator = entities.begin(); entities_iterator != entities.end(); entities_iterator++) {
        MovingEntity * moving_entity = entities_iterator->first;
        std::pair<std::multimap<std::pair<MovingEntity *, Entity *>, std::tr1::shared_ptr<Collision> >::iterator,
                std::multimap<std::pair<MovingEntity *, Entity *>, std::tr1::shared_ptr<Collision> >::iterator> range = entities_to_collisions.equal_range(*entities_iterator);
        // count how many edges intersect with each vertex to detect vertex-vertex collisions.
        std::set<std::pair<Entity *, int> > entity_vertexes;
        std::multimap<std::pair<Entity *, int>, std::tr1::shared_ptr<Collision> > entity_vertex_collisions;
        for (std::multimap<std::pair<MovingEntity *, Entity *>, std::tr1::shared_ptr<Collision> >::iterator collision_iterator = range.first; collision_iterator != range.second; collision_iterator++) {
            std::tr1::shared_ptr<Collision> collision = collision_iterator->second;
            std::pair<Entity *, int> pair(collision->vertex_entity, collision->vertex_index);
            entity_vertexes.insert(pair);
            Util::insert(&entity_vertex_collisions, pair, collision);
        }
        std::set<Vector2> collision_normals;
        for (std::set<std::pair<Entity *, int> >::iterator entity_vertex_iterator = entity_vertexes.begin(); entity_vertex_iterator != entity_vertexes.end(); entity_vertex_iterator++) {
            std::pair<std::multimap<std::pair<Entity *, int>, std::tr1::shared_ptr<Collision> >::iterator, std::multimap<std::pair<Entity *, int>, std::tr1::shared_ptr<Collision> >::iterator>
                    entity_vertex_collision_range = entity_vertex_collisions.equal_range(*entity_vertex_iterator);
            int count = std::distance(entity_vertex_collision_range.first, entity_vertex_collision_range.second);
            std::multimap<std::pair<Entity *, int>, std::tr1::shared_ptr<Collision> >::iterator entity_vertex_collision_iterator = entity_vertex_collision_range.first;
            if (count == 1) {
                // vertex-edge collision. easy to understand.
                collision_normals.insert(Util::normalized(entity_vertex_collision_iterator->second->getNormal()));
            } else if (count == 2) {
                // this is a vertex-vertex collision. oh dear.
                std::tr1::shared_ptr<Collision> participating_collisions[4];
                participating_collisions[0] = entity_vertex_collision_iterator->second;
                entity_vertex_collision_iterator++;
                participating_collisions[1] = entity_vertex_collision_iterator->second;
                // select the index of the edge with the greater index (and account for wrap around)
                int other_vertex_index = participating_collisions[ //
                        (participating_collisions[0]->edge_index + 1) % participating_collisions[0]->edge_entity->bounding_prismoid.size() == participating_collisions[1]->edge_index ? 1 : 0 //
                        ]->edge_index;
                std::pair<Entity *, int> other_pair(participating_collisions[0]->edge_entity, other_vertex_index);
                std::pair<std::multimap<std::pair<Entity *, int>, std::tr1::shared_ptr<Collision> >::iterator, std::multimap<std::pair<Entity *, int>, std::tr1::shared_ptr<Collision> >::iterator>
                        other_range = entity_vertex_collisions.equal_range(other_pair);
                Util::assert(std::distance(other_range.first, other_range.second) == 2, "dbf71330b0fabca40121e5fda23a47e5");
                std::multimap<std::pair<Entity *, int>, std::tr1::shared_ptr<Collision> >::iterator other_iterator = other_range.first;
                participating_collisions[2] = other_iterator->second;
                other_iterator++;
                participating_collisions[3] = other_iterator->second;

                // at this point, we have something like this:
                // std::cout << s(participating_collisions) << std::endl;
                //     (M.0(<32,0>), S.0(<0,0>,<32,0>), <0,-32>)
                //     (M.0(<32,0>), S.1(<32,0>,<32,32>), <32,-0>)
                //     (S.1(<32,0>), M.3(<32,32>,<32,0>), <32,0>)
                //     (S.1(<32,0>), M.0(<32,0>,<64,0>), <-0,32>)
                std::set<Vector2> competing_normals;
                // TODO: the order here is not deterministic.
                // TODO: Check to make sure we're pairing up the proper ones against each other.
                // TODO: A good solution would be to split participating_collisions into two arrays, and order the members before getting here.
                for (int i = 0; i < 2; i++) {
                    Vector2 left_normal = Util::normalized(participating_collisions[i + 0]->getNormal());
                    Vector2 right_normal = Util::normalized(participating_collisions[i + 2]->getNormal());
                    float relative_angle = Util::dot(left_normal, Util::perp(right_normal));
                    if ((relative_angle < 0) == (i == 0))
                        competing_normals.insert(left_normal);
                    else
                        competing_normals.insert(right_normal);
                }
                if (competing_normals.size() == 1)
                    collision_normals.insert(*competing_normals.begin());
                else
                    Util::assert(false, "TODO aaef45fcdf8136f6dedb1e7b09e3e422");
            } else {
                Util::assert(false, "c5196591ea2d8ad7e15d79f7cff263bb");
            }
        }
        Util::assert(collision_normals.size() == 1, "TODO e48291511ee88e8538f84e060bd4c8c1");
        // time to make decisions about these entities colliding.
        Vector2 normal = *collision_normals.begin();
        Vector2 normal_component = Util::dot(moving_entity->velocity, normal) * normal;
        Vector2 force = -normal_component;
        moving_entity->velocity += force;
        std::cout << s(normal) << " -> " << s(moving_entity->velocity) << std::endl;
    }
}

void Physics::invalidateCollisions(MovingEntity *entity)
{
    // mark any other collisions invalid
    std::pair<std::multimap<MovingEntity *, std::tr1::shared_ptr<Collision> >::iterator, std::multimap<MovingEntity *, std::tr1::shared_ptr<Collision> >::iterator> range =
            collisions_by_entity.equal_range(entity);
    for (std::multimap<MovingEntity *, std::tr1::shared_ptr<Collision> >::iterator iterator = range.first; iterator != range.second; iterator++)
        iterator->second->valid = false;
    collisions_by_entity.erase(range.first, range.second);
    if (false) {
        // TODO: put it back in the list
        entity->calculateBoundingPrismoid();
        detectCollisions(entity);
    }
}

Rectangle Physics::getBoundingRectangle(const Prismoid &prismoid)
{
    Edge edge;
    // initialize min and max with just any value that's in the prismoid.
    prismoid.getEdge(0, &edge);
    Vector2 min(edge.points[0].x, edge.points[0].y);
    Vector2 max = min;
    for (int i = 0; i < prismoid.size(); i++) {
        prismoid.getEdge(i, &edge);
        for (int z = 0; z < 2; z++) {
            float x = edge.points[z].x;
            float y = edge.points[z].y;
            min.x = Util::min(min.x, x);
            min.y = Util::min(min.y, y);
            max.x = Util::max(max.x, x);
            max.y = Util::max(max.y, y);
        }
    }
    return Rectangle(min.x, min.y, max.x, max.y);
}
