#include "game.hpp"

#include "input.hpp"
#include "rat64.hpp"

List<Entity> entities;

void game_init() {
    entities.append({ Entity::YOU,
        {{33000, 0}, {24000, 85000}},
        {-1000, 1000},
    });

    entities.append({ Entity::WALL,
        {{32000, 86000}, {32000, 32000}},
        {0, 0},
    });
    entities.append({ Entity::WALL,
        {{0, 86000}, {32000, 32000}},
        {0, 0},
    });
}

struct Collision {
    enum Orientation {
        UP,
        LEFT,
        DOWN,
        RIGHT,
    };

    rat64 time;
    Orientation orientation;
    int entity_index1;
    int entity_index2;
};
int compare_collisions(Collision a, Collision b) {
    return operator_compare(a.time, b.time);
}

// return iff there is an impact
static bool do_collision_vertical(const EdgeH & edge1, const Coord & velocity1, const EdgeH & edge2, const Coord & velocity2, rat64 * out_time_to_impact) {
    int64_t y1_start = edge1.position.y;
    int64_t y1_end = y1_start + velocity1.y;
    int64_t y2_start = edge2.position.y;
    int64_t y2_end = y2_start + velocity2.y;
    if (!is_line_segment_overlap(y1_start, y1_end, y2_start, y2_end))
        return false;
    // TODO: hard coded to assume only object 1 is moving
    rat64 time_to_impact = rat64::normalized(y2_start - y1_start, y1_end - y1_start);

    // where will the x's be at the time of impact?
    int64_t final_position_x1 = edge1.position.x + velocity1.x * time_to_impact.numerator / time_to_impact.denominator;
    int64_t final_position_x2 = edge2.position.x + velocity2.x * time_to_impact.numerator / time_to_impact.denominator;

    // make sure the x's allow some contact
    if (!is_line_segment_overlap(final_position_x1, final_position_x1 + edge1.size,
                                 final_position_x2, final_position_x2 + edge2.size)) {
        return false;
    }

    *out_time_to_impact = time_to_impact;
    return true;
}
static bool do_collision_horizontal(const EdgeV & edge1, const Coord & velocity1, const EdgeV & edge2, const Coord & velocity2, rat64 * out_time_to_impact) {
    int64_t x1_start = edge1.position.x;
    int64_t x1_end = x1_start + velocity1.x;
    int64_t x2_start = edge2.position.x;
    int64_t x2_end = x2_start + velocity2.x;
    if (!is_line_segment_overlap(x1_start, x1_end, x2_start, x2_end))
        return false;
    // TODO: hard coded to assume only object 1 is moving
    rat64 time_to_impact = rat64::normalized(x2_start - x1_start, x1_end - x1_start);

    // where will the y's be at the time of impact?
    int64_t final_position_y1 = edge1.position.y + velocity1.y * time_to_impact.numerator / time_to_impact.denominator;
    int64_t final_position_y2 = edge2.position.y + velocity2.y * time_to_impact.numerator / time_to_impact.denominator;

    // make sure the y's allow some contact
    if (!is_line_segment_overlap(final_position_y1, final_position_y1 + edge1.size,
                                 final_position_y2, final_position_y2 + edge2.size)) {
        return false;
    }

    *out_time_to_impact = time_to_impact;
    return true;
}
static void get_collisions(int entity_index1, int entity_index2, List<Collision> * collisions) {
    const Entity & entity1 = entities[entity_index1];
    const Entity & entity2 = entities[entity_index2];
    Coord relative_velocity = entity1.velocity - entity2.velocity;
    if (relative_velocity.y < 0) {
        EdgeH edge1 = get_top_edge(entity1.bounds);
        EdgeH edge2 = get_bottom_edge(entity2.bounds);
        rat64 time_to_impact;
        if (do_collision_vertical(edge1, entity1.velocity, edge2, entity2.velocity, &time_to_impact))
            collisions->append(Collision{time_to_impact, Collision::UP, entity_index1, entity_index2});
    }
    if (relative_velocity.x < 0) {
        EdgeV edge1 = get_left_edge(entity1.bounds);
        EdgeV edge2 = get_right_edge(entity2.bounds);
        rat64 time_to_impact;
        if (do_collision_horizontal(edge1, entity1.velocity, edge2, entity2.velocity, &time_to_impact))
            collisions->append(Collision{time_to_impact, Collision::LEFT, entity_index1, entity_index2});
    }
    if (relative_velocity.y > 0) {
        EdgeH edge1 = get_bottom_edge(entity1.bounds);
        EdgeH edge2 = get_top_edge(entity2.bounds);
        rat64 time_to_impact;
        if (do_collision_vertical(edge1, entity1.velocity, edge2, entity2.velocity, &time_to_impact))
            collisions->append(Collision{time_to_impact, Collision::DOWN, entity_index1, entity_index2});
    }
    if (relative_velocity.x > 0) {
        EdgeV edge1 = get_right_edge(entity1.bounds);
        EdgeV edge2 = get_left_edge(entity2.bounds);
        rat64 time_to_impact;
        if (do_collision_horizontal(edge1, entity1.velocity, edge2, entity2.velocity, &time_to_impact))
            collisions->append(Collision{time_to_impact, Collision::RIGHT, entity_index1, entity_index2});
    }
}

static void do_collisions() {
    List<Collision> collisions;
    for (int i = 0; i < entities.length(); i++) {
        const Entity & entity1 = entities[i];
        if (entity1.type == Entity::WALL) continue;
        for (int j = 0; j < entities.length(); j++) {
            if (i == j) continue;
            get_collisions(i, j, &collisions);
        }
    }

    sort<Collision, compare_collisions>(collisions.raw(), collisions.length());
    if (collisions.length() > 0) {
        Collision collision = collisions[0];
        Entity * entity = &entities[collision.entity_index1];
        switch (collision.orientation) {
            case Collision::UP:
            case Collision::DOWN:
                entity->bounds.position.y += entity->velocity.y * collision.time.numerator / collision.time.denominator;
                entity->velocity.y = 0;
                break;
            case Collision::LEFT:
            case Collision::RIGHT:
                entity->bounds.position.x += entity->velocity.x * collision.time.numerator / collision.time.denominator;
                entity->velocity.x = 0;
                break;
            default: panic("orientation");
        }
    }
}

void run_the_game() {
    Coord acceleration = {0, 0};
    if (input_state[INPUT_UP])
        acceleration.y -= 100;
    if (input_state[INPUT_LEFT])
        acceleration.x -= 100;
    if (input_state[INPUT_DOWN])
        acceleration.y += 100;
    if (input_state[INPUT_RIGHT])
        acceleration.x += 100;
    // assume you is always 0
    entities[0].velocity += acceleration;

    do_collisions();

    for (int i = 0; i < entities.length(); i++) {
        Entity * entity = &entities[i];
        entity->bounds.position += entity->velocity;
    }
}
