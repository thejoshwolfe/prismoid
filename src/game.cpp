#include "game.hpp"

#include "input.hpp"

Entity you;
List<Rect> walls;

void game_init() {
    you.bounds.position.x = 16000;
    you.bounds.size = {24000, 85000};

    walls.append(Rect{{0, 100000}, {32000, 32000}});
    walls.append(Rect{{33000, 99000}, {32000, 32000}});
}

// return iff there is an impact
static bool do_collision(const EdgeH & edge1, const Coord & velocity1, int64_t * out_distance_to_impact1,
                         const EdgeH & edge2, const Coord & velocity2, int64_t * out_distance_to_impact2)
{
    int64_t y1_start = edge1.position.y;
    int64_t y1_end = y1_start + velocity1.y;
    int64_t y2_start = edge2.position.y;
    int64_t y2_end = y2_start + velocity2.y;
    if (!is_line_segment_overlap(y1_start, y1_end, y2_start, y2_end))
        return false;
    // TODO: also check the x's

    // TODO: hard coded
    *out_distance_to_impact1 = y2_start - y1_start;
    *out_distance_to_impact2 = 0;
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
        int64_t closest_distance_to_impact;
        List<int> closest_wall_indexes;
        for (int i = 0; i < walls.length(); i++) {
            int64_t distance_to_impact;
            int64_t _unused; // TODO: hardcoded
            if (!do_collision(moving_edge, you.velocity, &distance_to_impact, get_top_edge(walls[i]), Coord{0, 0}, &_unused))
                continue;
            if (closest_wall_indexes.length() == 0) {
                // found an impact
                closest_distance_to_impact = distance_to_impact;
                closest_wall_indexes.append(i);
            } else if (distance_to_impact < closest_distance_to_impact) {
                // found a better impact
                closest_distance_to_impact = distance_to_impact;
                closest_wall_indexes.clear();
                closest_wall_indexes.append(i);
            } else if (distance_to_impact == closest_distance_to_impact) {
                // found a tied impact
                closest_wall_indexes.append(i);
            } else {
                // too far away. don't care.
            }
        }
        if (closest_wall_indexes.length() > 0) {
            you.bounds.position.y += closest_distance_to_impact;
            you.velocity.y = 0;
        }
    }
    // TODO: other directions

    you.bounds.position += you.velocity;
}
