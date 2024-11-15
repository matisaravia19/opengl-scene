#include "physicsWorld.h"

PhysicsWorld* PhysicsWorld::instance = nullptr;

PhysicsWorld* PhysicsWorld::getInstance() {
    if (!instance) {
        instance = new PhysicsWorld();
    }
    return instance;
}

PhysicsWorld::PhysicsWorld() {
    broadphase = new btDbvtBroadphase();

    collisionConfiguration = new btDefaultCollisionConfiguration();

    dispatcher = new btCollisionDispatcher(collisionConfiguration);

    solver = new btSequentialImpulseConstraintSolver();

    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

    dynamicsWorld->setGravity(btVector3(0.0f, -9.81f, 0.0f));
}

PhysicsWorld::~PhysicsWorld() {
    delete dynamicsWorld;
    delete solver;
    delete dispatcher;
    delete collisionConfiguration;
    delete broadphase;
}

btDiscreteDynamicsWorld* PhysicsWorld::getDynamicsWorld() {
    return dynamicsWorld;
}
