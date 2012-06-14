#include "Game.h"

#include "PlayerEntity.h"

Game::Game(std::string filename)
{
    PlayerEntity * player = new PlayerEntity(Vector2(100, -50));
    moving_entities.push_back(player);

    TiledTmx * map = TiledTmx::load(filename);
    Util::assert(tileset_image.LoadFromFile(map->tilesetImageFilename()), "image load");
    tile_size = map->tileSize();
    layer_width = map->width();
    layer_height = map->height();
    physics_layer = new StaticEntity*[layer_width * layer_height];
    for (int y = 0; y < layer_height; y++) {
        for (int x = 0; x < layer_width; x++) {
            Tile tile = map->tile(x, y);
            StaticEntity * entity = NULL;
            if (tile.global_id() != 0) {
                // we have to create subimages manually. see https://github.com/thejoshwolfe/grinch/issues/2
                sf::IntRect sub_rect = map->tilesetImageOffset(tile.global_id());
                sf::Image* image;
                if (tileset_images.count(sub_rect)) {
                    image = tileset_images[sub_rect];
                } else {
                    image = new sf::Image(sub_rect.GetWidth(), sub_rect.GetHeight());
                    image->Copy(tileset_image, 0, 0, sub_rect);
                    tileset_images[sub_rect] = image;
                }
                entity = new StaticEntity(Vector2(x * tile_size + tile_size / 2, y * tile_size + tile_size / 2), Vector2(tile_size, tile_size), *image, tile.is_flipped_horizontally(),
                        tile.is_flipped_vertically(), tile.is_flipped_diagonally());
            }
            physics_layer[y * layer_width + x] = entity;
        }
    }
    delete map;
}

void Game::doFrame(const sf::Input * input)
{
    for (int i = 0; i < (int)moving_entities.size(); i++)
        moving_entities[i]->frame_progress = 0;

    // run controllers
    this->input = input;
    for (int i = 0; i < (int)moving_entities.size(); i++)
        moving_entities[i]->doController(this);

    // first pass
    // determine desired motion
    for (int i = 0; i < (int)moving_entities.size(); i++)
        moving_entities[i]->calculateBoundingPrismoid();
    // detect collisions
    for (int i = 0; i < (int)moving_entities.size(); i++)
        detectCollisions(moving_entities[i]);

    // move everything in the right order
    while (!collisions_by_time.empty()) {
        float time = collisions_by_time.top().key;
        std::vector<std::tr1::shared_ptr<Collision> > collisions;
        while (!collisions_by_time.empty() && collisions_by_time.top().key == time) {
            std::tr1::shared_ptr<Collision> collision = collisions_by_time.top().value;
            if (collision->valid)
                collisions.push_back(collision);
            collisions_by_time.pop();
        }
        if (!collisions.empty())
            doCollision(time, collisions);
    }
    collisions_by_entity.clear();
}

void Game::detectCollisions(MovingEntity * entity)
{
    bool ever_added = false;

    // only check static entities in the tiles we'll be intersecting.
    Rectangle bounding_rectangle = getBoundingRectangle(entity->bounding_prismoid);
    std::vector<StaticEntity *> static_entities;
    getStaticEntities(static_entities, bounding_rectangle);
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
        maybeAddCollision(1, entity, NULL, Vector2(), Vector2(), Vector2());
    }
}

bool Game::detectCollision(MovingEntity *entity, Entity *other)
{
    bool ever_added = false;
    for (int i = 0; i < other->bounding_prismoid.size(); i++) {
        Edge other_edge1, other_edge2;
        other->bounding_prismoid.getFace(i, &other_edge1, &other_edge2);
        for (int j = 0; j < entity->bounding_prismoid.size(); j++) {
            Edge this_edge;
            entity->bounding_prismoid.getEdge(j, &this_edge);
            Vector3 collision_point;
            bool is_collision = getEdgeIntersectionWithQuadrilateral(this_edge, other_edge1, other_edge2, &collision_point);
            if (!is_collision)
                continue;
            Vector2 normal = other->bounding_prismoid.getNormal(i);
            Vector2 this_adjacent_edge1, this_adjacent_edge2;
            entity->bounding_prismoid.getAdjacentCapEdgeVectors(j, &this_adjacent_edge1, &this_adjacent_edge2);
            ever_added |= maybeAddCollision(collision_point.z, entity, other, normal, this_adjacent_edge1, this_adjacent_edge2);
        }
    }
    for (int i = 0; i < entity->bounding_prismoid.size(); i++) {
        Edge this_edge1, this_edge2;
        entity->bounding_prismoid.getFace(i, &this_edge1, &this_edge2);
        for (int j = 0; j < other->bounding_prismoid.size(); j++) {
            Edge other_edge;
            other->bounding_prismoid.getEdge(j, &other_edge);
            Vector3 collision_point;
            bool is_collision = getEdgeIntersectionWithQuadrilateral(other_edge, this_edge1, this_edge2, &collision_point);
            if (!is_collision)
                continue;
            Vector2 normal = entity->bounding_prismoid.getNormal(i);
            Vector2 other_adjacent_edge1, other_adjacent_edge2;
            other->bounding_prismoid.getAdjacentCapEdgeVectors(j, &other_adjacent_edge1, &other_adjacent_edge2);
            ever_added |= maybeAddCollision(collision_point.z, entity, other, -normal, -other_adjacent_edge1, -other_adjacent_edge2);
        }
    }
    return ever_added;
}

bool Game::getEdgeIntersectionWithQuadrilateral(const Edge &edge, const Edge &plane_edge1, const Edge &plane_edge2, Vector3 *output_intersection_point)
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
    Vector3 plane_points[] = {
            plane_edge1.points[0],
            plane_edge1.points[1],
            plane_edge2.points[1],
            plane_edge2.points[0],
    };
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

bool Game::maybeAddCollision(float time, MovingEntity *entity, Entity *other, const Vector2 &normal, const Vector2 &adjacent_edge1, const Vector2 &adjacent_edge2)
{
    // don't count it if we're moving away (which happens right after bouncing)
    // or if we're exactly parallel
    if (other != NULL && Util::dot(entity->velocity - other->getVelocity(), normal) >= 0)
        return false;
    // don't count it if the normal is impossible to reach this vertex.
    // this happens when the back corner scrapes by the backward-facing vertex of a floor tile.
    float normal_angle_limit = Util::dot(Util::normalized(adjacent_edge1), Util::normalized(adjacent_edge2));
    if (Util::dot(Util::normalized(normal), Util::normalized(adjacent_edge1)) < normal_angle_limit)
        return false;
    if (Util::dot(Util::normalized(normal), Util::normalized(adjacent_edge2)) < normal_angle_limit)
        return false;
    std::tr1::shared_ptr<Collision> collision(new Collision(entity, other, time, normal));
    Util::push(&collisions_by_time, time, collision);
    Util::insert(&collisions_by_entity, static_cast<Entity*>(entity), collision);
    if (other != NULL && other->is_moving_entity)
        Util::insert(&collisions_by_entity, other, collision);
    return true;
}

void Game::doCollision(float time, const std::vector<std::tr1::shared_ptr<Collision> > &collisions)
{
    std::tr1::shared_ptr<Collision> collision = collisions[0];
    MovingEntity * entity = collision->entity;
    Entity * other = collision->other;

    // snap to the collision point
    entity->center += Util::scaleVector(time - entity->frame_progress, entity->velocity);
    entity->frame_progress = time;

    if (other == NULL) {
        // not really a collision
        return;
    }

    if (other->is_moving_entity) {
        // snap the other to the collision point
        MovingEntity * other_moving_entity = static_cast<MovingEntity*>(other);
        other_moving_entity->center += Util::scaleVector(time - other_moving_entity->frame_progress, other_moving_entity->velocity);
        other_moving_entity->frame_progress = time;
    }

    // bounce
    Vector2 normal = Util::normalized(collision->normal);
    Vector2 relative_velocity = entity->velocity - other->getVelocity();
    Vector2 normal_component = Util::dot(relative_velocity, normal) * normal;
    Vector2 normal_force = -normal_component;
    Vector2 total_force = normal_force;
    entity->velocity += total_force;
    if (other->is_moving_entity) {
        // Newton's 3rd
        static_cast<MovingEntity*>(other)->velocity -= total_force;
    }

    // recalculate collisions for these entities
    invalidateCollisions(entity);
    if (other->is_moving_entity)
        invalidateCollisions(static_cast<MovingEntity*>(other));
}

void Game::invalidateCollisions(MovingEntity *entity)
{
    // mark any other collisions invalid
    std::pair<std::multimap<Entity *, std::tr1::shared_ptr<Collision> >::iterator, std::multimap<Entity *, std::tr1::shared_ptr<Collision> >::iterator> range =
            collisions_by_entity.equal_range(entity);
    for (std::multimap<Entity *, std::tr1::shared_ptr<Collision> >::iterator iterator = range.first; iterator != range.second; iterator++)
        iterator->second->valid = false;
    collisions_by_entity.erase(range.first, range.second);
    // put it back in the list
    entity->calculateBoundingPrismoid();
    detectCollisions(entity);
}

void Game::render(sf::RenderTarget *render_target)
{
    render_target->GetDefaultView().SetCenter(0, 0);
    Vector2 virtual_center = moving_entities[0]->center;

    float width_apothem = render_target->GetWidth() / 2;
    float height_apothem = render_target->GetHeight() / 2;
    Rectangle bounding_rectangle( //
            virtual_center.x - width_apothem, virtual_center.y - height_apothem, //
            virtual_center.x + width_apothem, virtual_center.y + height_apothem);
    std::vector<StaticEntity *> static_entities;
    getStaticEntities(static_entities, bounding_rectangle);
    for (int i = 0; i < (int)static_entities.size(); i++)
        static_entities[i]->render(virtual_center, render_target);
    for (int i = 0; i < (int)moving_entities.size(); i++)
        moving_entities[i]->render(virtual_center, render_target);
}

Rectangle Game::getBoundingRectangle(const Prismoid &prismoid)
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

void Game::getStaticEntities(std::vector<StaticEntity *> &static_entities, const Rectangle &bounding_rectangle)
{
    int min_x = Util::toTileIndexFloored(bounding_rectangle.Left, tile_size);
    int min_y = Util::toTileIndexFloored(bounding_rectangle.Top, tile_size);
    int max_x = Util::toTileIndexCeilinged(bounding_rectangle.Right, tile_size);
    int max_y = Util::toTileIndexCeilinged(bounding_rectangle.Bottom, tile_size);

    min_x = Util::max(min_x, 0);
    min_y = Util::max(min_y, 0);
    max_x = Util::min(max_x, layer_width - 1);
    max_y = Util::min(max_y, layer_height - 1);

    for (int y = min_y; y <= max_y; y++) {
        for (int x = min_x; x <= max_x; x++) {
            StaticEntity * entity = physics_layer[y * layer_width + x];
            if (entity != NULL)
                static_entities.push_back(entity);
        }
    }
}
