#include "game.hpp"

#include "input.hpp"
#include "rat64.hpp"

Entity you;
List<Rect> walls;

void game_init() {
    you.bounds.position.x = 0;
    you.bounds.size = {24000, 85000};
    you.velocity = {-6000, 3000};

    walls.append(Rect{{23000, 86000}, {32000, 32000}});
}

// return iff there is an impact
static bool do_collision(const EdgeH & edge1, const Coord & velocity1, const EdgeH & edge2, const Coord & velocity2, rat64 * out_time_to_impact) {
    int64_t y1_start = edge1.position.y;
    int64_t y1_end = y1_start + velocity1.y;
    int64_t y2_start = edge2.position.y;
    int64_t y2_end = y2_start + velocity2.y;
    if (!is_line_segment_overlap(y1_start, y1_end, y2_start, y2_end))
        return false;
    // TODO: hard coded to assume only object 1 is moving
    rat64 time_to_impact = rat64(y2_start - y1_start, y1_end - y1_start);

    // scale down velocities and recompute end points
    int64_t real_velocity_x1 = velocity1.x * time_to_impact.numerator / time_to_impact.denominator;
    int64_t real_velocity_x2 = velocity2.x * time_to_impact.numerator / time_to_impact.denominator;
    int64_t final_position_x1 = edge1.position.x + real_velocity_x1;
    int64_t final_position_x2 = edge2.position.x + real_velocity_x2;

    // make sure the x's allow some contact
    if (!is_line_segment_overlap(final_position_x1, final_position_x1 + edge1.size,
                                 final_position_x2, final_position_x2 + edge2.size)) {
        return false;
    }

    *out_time_to_impact = time_to_impact;
    return true;
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

    // collisions
    if (you.velocity.y > 0) {
        EdgeH moving_edge = get_bottom_edge(you.bounds);
        rat64 soonest_time_to_impact;
        List<int> closest_wall_indexes;
        for (int i = 0; i < walls.length(); i++) {
            rat64 time_to_impact;
            if (!do_collision(moving_edge, you.velocity, get_top_edge(walls[i]), Coord{0, 0}, &time_to_impact))
                continue;
            if (closest_wall_indexes.length() == 0) {
                // found an impact
                soonest_time_to_impact = time_to_impact;
                closest_wall_indexes.append(i);
            } else if (time_to_impact < soonest_time_to_impact) {
                // found a better impact
                soonest_time_to_impact = time_to_impact;
                closest_wall_indexes.clear();
                closest_wall_indexes.append(i);
            } else if (time_to_impact == soonest_time_to_impact) {
                // found a tied impact
                closest_wall_indexes.append(i);
            } else {
                // too far away. don't care.
            }
        }
        if (closest_wall_indexes.length() > 0) {
            you.bounds.position.y += you.velocity.y * soonest_time_to_impact.numerator / soonest_time_to_impact.denominator;
            you.velocity.y = 0;
        }
    }
    // TODO: other directions

    you.bounds.position += you.velocity;
}
