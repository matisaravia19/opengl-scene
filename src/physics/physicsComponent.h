#pragma once

#include "../core/component.h"
#include "btBulletDynamicsCommon.h"
#include "glm/glm.hpp"
#include "../core/entity.h"

class PhysicsComponent : public Component {
private:
    Transform *transform;

    btRigidBody *rigidBody;
    btCollisionShape *collisionShape;
    btMotionState *motionState;
    btScalar mass;
    bool isDynamic;
    btTransform initialTransform;

    void cleanup();

public:
    PhysicsComponent(float mass = 1.0f, bool isDynamic = true);
    ~PhysicsComponent() override;

    void init() override;
    void update() override;
    void remove() override;

    void setLinearVelocity(const glm::vec3 &velocity);
    void setAngularVelocity(const glm::vec3 &velocity);
    void applyForce(const glm::vec3 &force, const glm::vec3 &relativePos = glm::vec3(0.0f));
    void applyImpulse(const glm::vec3 &impulse, const glm::vec3 &relativePos = glm::vec3(0.0f));

    void setFriction(float friction);
    void setRestitution(float restitution);

    btRigidBody *getRigidBody() { return rigidBody; }
};