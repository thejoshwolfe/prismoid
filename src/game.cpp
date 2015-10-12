#include "game.hpp"

#include "input.hpp"
#include "rat64.hpp"

List<Entity> entities;

void game_init() {
    entities.append({ Entity::YOU,
        {{1000, -86000}, {24000, 85000}},
        {-1100, 3000},
    });

    entities.append({ Entity::WALL,
        {{-32000, -85000 - 31000}, {32000, 32000}},
        {0, 0},
    });
    entities.append({ Entity::WALL,
        {{24000, 0}, {32000, 32000}},
        {0, 0},
    });
}

struct Collision {
    enum Orientation {
        UP    = 1,
        LEFT  = 2,
        DOWN  = 4,
        RIGHT = 8,
        UP_LEFT    = UP    | LEFT,
        LEFT_DOWN  = LEFT  | DOWN,
        DOWN_RIGHT = DOWN  | RIGHT,
        RIGHT_UP   = RIGHT | UP,
    };

    rat64 time;
    Orientation orientation;
    int entity_index1;
    int entity_index2;
};
static inline int get_orientation_score(Collision::Orientation orientation) {
    switch (orientation) {
        case Collision::UP:
        case Collision::LEFT:
        case Collision::DOWN:
        case Collision::RIGHT:
            return 0; // better
        case Collision::UP_LEFT:
        case Collision::LEFT_DOWN:
        case Collision::DOWN_RIGHT:
        case Collision::RIGHT_UP:
            return 1; // worse
    }
    panic("orientation");
}
static inline int compare_collisions(Collision a, Collision b) {
    int result;
    if ((result = operator_compare(a.time, b.time)) != 0) return result;
    if ((result = operator_compare(get_orientation_score(a.orientation), get_orientation_score(b.orientation))) != 0) return result;
    return 0;
}

static inline int get_line_segment_overlap(int64_t start1, int64_t end1, int64_t start2, int64_t end2) {
    // start must not be greater than end.
    // returns:
    //  -2: miss to the left/top
    //  -1: hit left/top corner
    //   0: hit edge-to-edge
    //   1: hit right/bottom corner
    //   2: miss to the right/bottom
    int alignment1 = sign(start1 - end2);
    int alignment2 = sign(end1 - start2);
    return alignment1 + alignment2;
}

static void get_collisions(int entity_index1, int entity_index2, List<Collision> * collisions) {
    const Entity & entity1 = entities[entity_index1];
    const Entity & entity2 = entities[entity_index2];
    // can you see the chalice in the following 8 lines of code?
    EdgeH top_edge1    =    get_top_edge(entity1.bounds);
    EdgeH top_edge2    =    get_top_edge(entity2.bounds);
    EdgeV left_edge1   =   get_left_edge(entity1.bounds);
    EdgeV left_edge2   =   get_left_edge(entity2.bounds);
    EdgeH bottom_edge1 = get_bottom_edge(entity1.bounds);
    EdgeH bottom_edge2 = get_bottom_edge(entity2.bounds);
    EdgeV right_edge1  =  get_right_edge(entity1.bounds);
    EdgeV right_edge2  =  get_right_edge(entity2.bounds);

    // calculate time to impact in each direction.
    // 0/1(0) means we start in contact and we're moving into it or away from it
    // 0/0(nan) means we're going to be in contact for the entire physics frame.
    // 1/1(1) means we're going to touch it at the end of the physics frame.
    // 1/2 means we'll get there halfway through the physics frame.
    // 3/2 means we'll get there later maybe, but not in this physics frame.
    // -5/1 means we're not in contact, and we're moving in the wrong direction.
    // 1/0(infinity) means we're not in contact, and not moving to get there either.
    Coord relative_velocity = entity1.velocity - entity2.velocity;
    rat64 time_to_impact_top    = rat64::normalized(bottom_edge2.position.y - top_edge1.position.y, relative_velocity.y);
    rat64 time_to_impact_left   = rat64::normalized(right_edge2.position.x - left_edge1.position.x, relative_velocity.x);
    rat64 time_to_impact_bottom = rat64::normalized(top_edge2.position.y - bottom_edge1.position.y, relative_velocity.y);
    rat64 time_to_impact_right  = rat64::normalized(left_edge2.position.x - right_edge1.position.x, relative_velocity.x);

    // turn nan into 0. we may be touching the whole time, but let's just pick one time to make our math easier.
    if (time_to_impact_top    == rat64::nan()) time_to_impact_top    = rat64{0,1};
    if (time_to_impact_left   == rat64::nan()) time_to_impact_left   = rat64{0,1};
    if (time_to_impact_bottom == rat64::nan()) time_to_impact_bottom = rat64{0,1};
    if (time_to_impact_right  == rat64::nan()) time_to_impact_right  = rat64{0,1};

    // which edges are close enough (in one axis) to collide?
    int vertical_orientation = 0;
    int horizontal_orientation = 0;
    EdgeH vertical_contact_edge1;
    EdgeH vertical_contact_edge2;
    EdgeV horizontal_contact_edge1;
    EdgeV horizontal_contact_edge2;
    rat64 vertical_time;
    rat64 horizontal_time;
    if (relative_velocity.y <= 0 && time_to_impact_top    >= rat64{0,1} && time_to_impact_top    <= rat64{1,1}) { vertical_orientation   = Collision::UP;      vertical_contact_edge1 =    top_edge1;   vertical_contact_edge2 = bottom_edge2; vertical_time   = time_to_impact_top;    }
    if (relative_velocity.x <= 0 && time_to_impact_left   >= rat64{0,1} && time_to_impact_left   <= rat64{1,1}) { horizontal_orientation = Collision::LEFT;  horizontal_contact_edge1 =   left_edge1; horizontal_contact_edge2 =  right_edge2; horizontal_time = time_to_impact_left;   }
    if (relative_velocity.y >= 0 && time_to_impact_bottom >= rat64{0,1} && time_to_impact_bottom <= rat64{1,1}) { vertical_orientation   = Collision::DOWN;    vertical_contact_edge1 = bottom_edge1;   vertical_contact_edge2 =    top_edge2; vertical_time   = time_to_impact_bottom; }
    if (relative_velocity.x >= 0 && time_to_impact_right  >= rat64{0,1} && time_to_impact_right  <= rat64{1,1}) { horizontal_orientation = Collision::RIGHT; horizontal_contact_edge1 =  right_edge1; horizontal_contact_edge2 =   left_edge2; horizontal_time = time_to_impact_right;  }

    // bounds check the other axis to distinguish between hitting and missing the target
    if (vertical_orientation != 0) {
        int64_t comparative_position_x1 = vertical_contact_edge1.position.x + relative_velocity.x * vertical_time.numerator / vertical_time.denominator;
        int overlap = get_line_segment_overlap(
                comparative_position_x1,           comparative_position_x1           + vertical_contact_edge1.size,
                vertical_contact_edge2.position.x, vertical_contact_edge2.position.x + vertical_contact_edge2.size);
        switch (overlap) {
            case -1: // target's left corner
                vertical_orientation |= Collision::RIGHT;
                break;
            case  0: // nailed it
                break;
            case  1: // target's right corner
                vertical_orientation |= Collision::LEFT;
                break;
            default: // miss
                vertical_orientation = 0;
                break;
        }
    }
    if (horizontal_orientation != 0) {
        int64_t comparative_position_y1 = horizontal_contact_edge1.position.y + relative_velocity.y * horizontal_time.numerator / horizontal_time.denominator;
        int overlap = get_line_segment_overlap(
                comparative_position_y1,             comparative_position_y1             + horizontal_contact_edge1.size,
                horizontal_contact_edge2.position.y, horizontal_contact_edge2.position.y + horizontal_contact_edge2.size);
        switch (overlap) {
            case -1: // target's top corner
                horizontal_orientation |= Collision::DOWN;
                break;
            case  0: // nailed it
                break;
            case  1: // target's bottom corner
                horizontal_orientation |= Collision::UP;
                break;
            default: // miss
                horizontal_orientation = 0;
                break;
        }
    }

    rat64 the_time_to_impact;
    if (vertical_orientation != 0)
        the_time_to_impact = vertical_time;
    else if (horizontal_orientation != 0)
        the_time_to_impact = horizontal_time;

    int orientation = vertical_orientation | horizontal_orientation;
    if (orientation != 0)
        collisions->append(Collision{the_time_to_impact, (Collision::Orientation)orientation, entity_index1, entity_index2});
}

static void do_collisions() {
    rat64 time_left_this_tick = {1,1};
    while (time_left_this_tick > rat64{0,1}) {
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

        rat64 time_of_impact_that_matters = rat64::nan();
        for (int i = 0; i < collisions.length(); i++) {
            Collision collision = collisions[i];
            if (time_of_impact_that_matters != rat64::nan() && time_of_impact_that_matters != collision.time) {
                time_left_this_tick -= time_of_impact_that_matters;
                goto start_over;
            }
            Entity * entity = &entities[collision.entity_index1];
            // try to turn diagonal collisions into orthogonal collisions
            switch (collision.orientation) {
                case Collision::UP_LEFT:
                case Collision::LEFT_DOWN:
                case Collision::DOWN_RIGHT:
                case Collision::RIGHT_UP: {
                    Coord relative_velocity = entity->velocity - entities[collision.entity_index2].velocity;
                    int64_t magnitude_x = abs(relative_velocity.x);
                    int64_t magnitude_y = abs(relative_velocity.y);
                    if (magnitude_x > magnitude_y) {
                        // let the x through
                        collision.orientation = (Collision::Orientation)(collision.orientation & ~(Collision::LEFT | Collision::RIGHT));
                    } else if (magnitude_x < magnitude_y) {
                        // let the y through
                        collision.orientation = (Collision::Orientation)(collision.orientation & ~(Collision::UP | Collision::DOWN));
                    } else {
                        // exactly even. dead stop.
                    }
                    break;
                }
                default: break;
            }
            switch (collision.orientation) {
                case Collision::UP:
                case Collision::DOWN:
                    if (entity->velocity.y != 0) {
                        entity->bounds.position.y += entity->velocity.y * collision.time.numerator / collision.time.denominator;
                        entity->velocity.y = 0;
                        time_of_impact_that_matters = collision.time;
                    }
                    break;
                case Collision::LEFT:
                case Collision::RIGHT:
                    if (entity->velocity.x != 0) {
                        entity->bounds.position.x += entity->velocity.x * collision.time.numerator / collision.time.denominator;
                        entity->velocity.x = 0;
                        time_of_impact_that_matters = collision.time;
                    }
                    break;
                case Collision::UP_LEFT:
                case Collision::LEFT_DOWN:
                case Collision::DOWN_RIGHT:
                case Collision::RIGHT_UP: {
                    if (entity->velocity.y != 0) {
                        entity->bounds.position.y += entity->velocity.y * collision.time.numerator / collision.time.denominator;
                        entity->velocity.y = 0;
                        time_of_impact_that_matters = collision.time;
                    }
                    if (entity->velocity.x != 0) {
                        entity->bounds.position.x += entity->velocity.x * collision.time.numerator / collision.time.denominator;
                        entity->velocity.x = 0;
                        time_of_impact_that_matters = collision.time;
                    }
                    break;
                }
                default: panic("orientation");
            }
        }
        // all collisions resolved
        break;
        start_over:;
    }

    for (int i = 0; i < entities.length(); i++) {
        Entity * entity = &entities[i];
        entity->bounds.position += entity->velocity;
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
}
