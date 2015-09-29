#include "game.hpp"

#include "input.hpp"
#include "rat64.hpp"

Entity you;
List<Rect> walls;

void game_init() {
    you.bounds.position.x = -1000;
    you.bounds.size = {24000, 85000};
    you.velocity = {1000, 3000};

    walls.append(Rect{{24000, 85000}, {32000, 32000}});
}

struct Collision {
    enum Orientation {
        UP,
        LEFT,
        DOWN,
        RIGHT,
    };

    rat64 time;
    int wall_index;
    Orientation orientation;
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
    rat64 time_to_impact = rat64(y2_start - y1_start, y1_end - y1_start);

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
    rat64 time_to_impact = rat64(x2_start - x1_start, x1_end - x1_start);

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

static void do_collisions() {
    List<Collision> collisions;
    if (you.velocity.y < 0) {
        EdgeH moving_edge = get_top_edge(you.bounds);
        for (int i = 0; i < walls.length(); i++) {
            rat64 time_to_impact;
            if (do_collision_vertical(moving_edge, you.velocity, get_bottom_edge(walls[i]), Coord{0, 0}, &time_to_impact))
                collisions.append(Collision{time_to_impact, i, Collision::UP});
        }
    }
    if (you.velocity.x < 0) {
        EdgeV moving_edge = get_left_edge(you.bounds);
        for (int i = 0; i < walls.length(); i++) {
            rat64 time_to_impact;
            if (do_collision_horizontal(moving_edge, you.velocity, get_right_edge(walls[i]), Coord{0, 0}, &time_to_impact))
                collisions.append(Collision{time_to_impact, i, Collision::LEFT});
        }
    }
    if (you.velocity.y > 0) {
        EdgeH moving_edge = get_bottom_edge(you.bounds);
        for (int i = 0; i < walls.length(); i++) {
            rat64 time_to_impact;
            if (do_collision_vertical(moving_edge, you.velocity, get_top_edge(walls[i]), Coord{0, 0}, &time_to_impact))
                collisions.append(Collision{time_to_impact, i, Collision::DOWN});
        }
    }
    if (you.velocity.x > 0) {
        EdgeV moving_edge = get_right_edge(you.bounds);
        for (int i = 0; i < walls.length(); i++) {
            rat64 time_to_impact;
            if (do_collision_horizontal(moving_edge, you.velocity, get_left_edge(walls[i]), Coord{0, 0}, &time_to_impact))
                collisions.append(Collision{time_to_impact, i, Collision::RIGHT});
        }
    }
    sort<Collision, compare_collisions>(collisions.raw(), collisions.length());
    if (collisions.length() > 0) {
        Collision collision = collisions[0];
        switch (collision.orientation) {
            case Collision::UP:
            case Collision::DOWN:
                you.bounds.position.y += you.velocity.y * collision.time.numerator / collision.time.denominator;
                you.velocity.y = 0;
                break;
            case Collision::LEFT:
            case Collision::RIGHT:
                you.bounds.position.x += you.velocity.x * collision.time.numerator / collision.time.denominator;
                you.velocity.x = 0;
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

    you.velocity += acceleration;

    do_collisions();

    you.bounds.position += you.velocity;
}
