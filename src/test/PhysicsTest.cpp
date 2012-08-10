#include "PhysicsTest.h"

void PhysicsTest::run()
{
    Physics physics(this);
    Vector2 center_start(32, 0);
    physics.moving_entities.push_back(new MovingEntity(center_start, Vector2(32, 32), Vector2(0, -1)));
    static_entities.push_back(new StaticEntity(Vector2(0, 0), Vector2(32, 32)));
    physics.doFrame();
    assert(physics.moving_entities[0]->center != center_start, "should move");
}

void PhysicsTest::assert(bool condition, std::string message)
{
    if (condition)
        return;
    std::cerr << "FAIL: " << message << std::endl;
    throw;
}
