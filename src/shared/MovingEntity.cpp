#include "MovingEntity.h"

void MovingEntity::calculateBoundingPrismoid()
{
    Vector2 remaining_velocity = Util::scaleVector(1 - frame_progress, velocity);
    std::vector<Vector2> here;
    Util::makeRectangle(&here, center, size);

    bounding_prismoid.clearEdges();
    bounding_prismoid.setZ(frame_progress, 1);
    for (int i = 0; i < (int)here.size(); i++) {
        Vector2 point = here[i];
        bounding_prismoid.addEdge(point, point + remaining_velocity);
    }
}
