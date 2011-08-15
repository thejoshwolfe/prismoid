#include "Game.h"

#include "PlayerEntity.h"

Game::Game() :
    frame_counter(0)
{
    moving_entities.push_back(new PlayerEntity(sf::Vector2f(30, 0), sf::Vector2f(30, 30), sf::Color::Blue, 0.5, 1.25, sf::Vector2f(0, 0)));
    moving_entities.push_back(new MovingEntity(sf::Vector2f(30, -100), sf::Vector2f(30, 30), sf::Color::Magenta, 0.5, 1.25, sf::Vector2f(0, 0)));

    static_entities.push_back(new StaticEntity(sf::Vector2f(0, 0), sf::Vector2f(20, 600), sf::Color::Red, 0.5, 0.25));
    static_entities.push_back(new StaticEntity(sf::Vector2f(250, 305), sf::Vector2f(500, 10), sf::Color::Red, 0.5, 0.25));
    static_entities.push_back(new StaticEntity(sf::Vector2f(500, 0), sf::Vector2f(20, 600), sf::Color::Red, 0.5, 0.25));
}

void Game::doFrame(const sf::Input * input)
{
    frame_counter++;

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
        std::tr1::shared_ptr<Collision> collision = collisions_by_time.top().value;
        collisions_by_time.pop();
        if (collision->valid)
            doCollision(time, collision);
    }
    collisions_by_entity.clear();
}

void Game::detectCollisions(MovingEntity * entity)
{
    bool ever_added = false;
    for (int j = 0; j < (int)static_entities.size(); j++)
        ever_added |= detectCollision(entity, static_entities[j]);
    for (int j = 0; j < (int)moving_entities.size(); j++) {
        MovingEntity * other_entity = moving_entities[j];
        if (entity != other_entity)
            ever_added |= detectCollision(entity, other_entity);
    }
    if (!ever_added) {
        // add a fake collision so that the entity will be updated at all
        sf::Vector2f dummy_normal;
        maybeAddCollision(1, entity, NULL, dummy_normal);
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
            sf::Vector3f collision_point;
            bool is_collision = Util::getEdgeIntersectionWithQuadrilateral(this_edge, other_edge1, other_edge2, &collision_point);
            if (!is_collision)
                continue;
            sf::Vector2f normal = other->bounding_prismoid.getNormal(i);
            ever_added |= maybeAddCollision(collision_point.z, entity, other, normal);
        }
    }
    for (int i = 0; i < entity->bounding_prismoid.size(); i++) {
        Edge this_edge1, this_edge2;
        entity->bounding_prismoid.getFace(i, &this_edge1, &this_edge2);
        for (int j = 0; j < other->bounding_prismoid.size(); j++) {
            Edge other_edge;
            other->bounding_prismoid.getEdge(j, &other_edge);
            sf::Vector3f collision_point;
            bool is_collision = Util::getEdgeIntersectionWithQuadrilateral(other_edge, this_edge1, this_edge2, &collision_point);
            if (!is_collision)
                continue;
            sf::Vector2f normal = -entity->bounding_prismoid.getNormal(i);
            ever_added |= maybeAddCollision(collision_point.z, entity, other, normal);
        }
    }
    return ever_added;
}

bool Game::maybeAddCollision(float time, MovingEntity *entity, Entity *other, const sf::Vector2f &normal)
{
    // don't count it if we're moving away (which happens right after bouncing)
    // or if we're exactly parallel
    if (other != NULL && Util::dot(entity->velocity - other->getVelocity(), normal) >= 0)
        return false;
    std::tr1::shared_ptr<Collision> collision(new Collision(entity, other, normal));
    Util::push(&collisions_by_time, time, collision);
    Util::insert(&collisions_by_entity, static_cast<Entity*>(entity), collision);
    if (other != NULL && other->is_moving_entity)
        Util::insert(&collisions_by_entity, other, collision);
    return true;
}

void Game::doCollision(float time, std::tr1::shared_ptr<Collision> collision)
{
    MovingEntity * entity = collision->entity;
    Entity * other = collision->other;

    // snap to the collision point
    entity->center += (time - entity->frame_progress) * entity->velocity;
    entity->frame_progress = time;

    if (other == NULL) {
        // not really a collision
        return;
    }

    if (other->is_moving_entity) {
        // snap the other to the collition point
        MovingEntity * other_moving_entity = static_cast<MovingEntity*>(other);
        other_moving_entity->center += (time - other_moving_entity->frame_progress) * other_moving_entity->velocity;
        other_moving_entity->frame_progress = time;
    }

    // bounce
    sf::Vector2f normal = Util::normalized(collision->normal);
    float elasticity = entity->elasticity * other->elasticity;
    sf::Vector2f relative_velocity = entity->velocity - other->getVelocity();
    sf::Vector2f normal_component = Util::dot(relative_velocity, normal) * normal;
    sf::Vector2f tangent_component = relative_velocity - normal_component;
    sf::Vector2f normal_force = -(1 + elasticity) * normal_component;
    float friction_coefficient = entity->friction * other->friction;
    float friction_magnitude = Util::min(friction_coefficient * Util::magnitude(normal_force), Util::magnitude(tangent_component));
    sf::Vector2f friction_force = -friction_magnitude * Util::normalized(tangent_component);
    sf::Vector2f total_force = normal_force + friction_force;
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
    std::pair<std::multimap<Entity *, std::tr1::shared_ptr<Collision> >::iterator, std::multimap<Entity *, std::tr1::shared_ptr<Collision> >::iterator> range = collisions_by_entity.equal_range(entity);
    for (std::multimap<Entity *, std::tr1::shared_ptr<Collision> >::iterator iterator = range.first; iterator != range.second; iterator++)
        iterator->second->valid = false;
    // put it back in the list
    entity->calculateBoundingPrismoid();
    detectCollisions(entity);
}

void Game::render(sf::RenderTarget *render_target)
{
    render_target->GetDefaultView().SetCenter(moving_entities[0]->center);

    for (int i = 0; i < (int)static_entities.size(); i++)
        static_entities[i]->render(render_target);
    for (int i = 0; i < (int)moving_entities.size(); i++)
        moving_entities[i]->render(render_target);
}

void Game::saveState(std::vector<byte>* buffer)
{
    Util::serialize(buffer, frame_counter);
    Util::serialize(buffer, (int)moving_entities.size());
    for (int i = 0; i < (int)moving_entities.size(); i++)
        moving_entities[i]->serialize(buffer);
}
void Game::loadState(std::vector<byte>::const_iterator* buffer)
{
    frame_counter = Util::deserialize<int64>(buffer);
    for (int i = 0; i < (int)moving_entities.size(); i++)
        delete moving_entities[i];
    moving_entities.clear();
    int moving_entities_size = Util::deserialize<int>(buffer);
    for (int i = 0; i < moving_entities_size; i++)
        moving_entities.push_back(MovingEntity::deserialize(buffer));
}
