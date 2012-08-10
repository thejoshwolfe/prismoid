#ifndef STATICENTITYPROVIDER_H
#define STATICENTITYPROVIDER_H

#include "StaticEntity.h"

class StaticEntityProvider
{
public:
    virtual void getStaticEntities(std::vector<StaticEntity *> &static_entities, const Rectangle &bounding_rectangle) = 0;
};

#endif
