#include "StaticEntity.h"

void StaticEntity::getBoundingPrismoid(Prismoid *prismoid)
{
    prismoid->setZ(0, 1);
    std::vector<sf::Vector2f> here;
    getBoundingPolygon(&here);
    for (int i = 0; i < (int)here.size(); i++)
        prismoid->addEdge(here[i], here[i]);
}
