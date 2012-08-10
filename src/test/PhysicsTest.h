#ifndef PHYSICSTEST_H
#define PHYSICSTEST_H

#include "Physics.h"

#include <iostream>

class PhysicsTest: public StaticEntityProvider
{
private:
    std::vector<StaticEntity*> static_entities;

    void assert(bool condition, std::string message);
public:
    void run();
    void getStaticEntities(std::vector<StaticEntity *> &static_entities, const Rectangle &bounding_rectangle)
    {
        static_entities = this->static_entities;
    }
};

#endif
