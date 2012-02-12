#include "MovingEntity.h"

#include "Game.h"
#include "PlayerEntity.h"

void MovingEntity::render(Vector2 virtual_center, sf::RenderTarget *render_target)
{
    if (false) {
        // draw motion bounding polygon
        sf::Shape shape;
        for (int i = 0; i < bounding_prismoid.size(); i++) {
            Edge edge;
            bounding_prismoid.getEdge(i, &edge);
            shape.AddPoint(Util::toRenderPoint(virtual_center, Vector2(edge.points[0].x, edge.points[0].y)), sf::Color::Green);
        }
        render_target->Draw(shape);
    }

    Entity::render(virtual_center, render_target);
}

void MovingEntity::doController(Game *)
{
    velocity.y += 0.4f;
}

void MovingEntity::calculateBoundingPrismoid()
{
    Vector2 remaining_velocity = (bigint)(1 - frame_progress) * velocity;
    std::vector<Vector2> here;
    makeRectangle(&here, center, size);

    bounding_prismoid.clearEdges();
    bounding_prismoid.setZ(frame_progress, 1);
    for (int i = 0; i < (int)here.size(); i++) {
        Vector2 point = here[i];
        bounding_prismoid.addEdge(point, point + remaining_velocity);
    }
}
