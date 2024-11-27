#pragma once

#include "btBulletDynamicsCommon.h"

class PhysicsWorld {
private:
    static PhysicsWorld *instance;

    btBroadphaseInterface *broadphase;
    btDefaultCollisionConfiguration *collisionConfiguration;
    btCollisionDispatcher *dispatcher;
    btSequentialImpulseConstraintSolver *solver;
    btDiscreteDynamicsWorld *dynamicsWorld;
public:
    static PhysicsWorld *getInstance();

    PhysicsWorld();
    ~PhysicsWorld();

    btDiscreteDynamicsWorld *getDynamicsWorld();
};