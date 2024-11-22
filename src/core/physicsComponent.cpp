#include "PhysicsComponent.h"
#include "PhysicsWorld.h"
#include <glm/gtc/type_ptr.hpp>
#include "../rendering/MeshRenderer.h"

PhysicsComponent::PhysicsComponent(float mass, bool isDynamic)
        : mass(mass), isDynamic(isDynamic), rigidBody(nullptr), collisionShape(nullptr), motionState(nullptr) {
}

PhysicsComponent::~PhysicsComponent() {
    cleanup();
}

void PhysicsComponent::cleanup() {
    if (rigidBody) {
        if (rigidBody->getMotionState()) {
            delete rigidBody->getMotionState();
        }
        delete rigidBody;
        rigidBody = nullptr;
    }
    if (collisionShape) {
        delete collisionShape;
        collisionShape = nullptr;
    }
    if (motionState) {
        delete motionState;
        motionState = nullptr;
    }
}

void PhysicsComponent::init() {
    Component::init();

    transform = getEntity()->getComponent<Transform>();
    auto mesh = getEntity()->getComponent<MeshRenderer>()->getMesh();

    glm::vec3 position = transform->getPosition();
    glm::vec3 scale = transform->getScale();
    glm::vec3 rotation = transform->getEulerAngles();

    glm::vec3 boundingBox = mesh->getBoundingBox();
    collisionShape = new btBoxShape(btVector3(boundingBox.x * scale.x, boundingBox.y * scale.y, boundingBox.z * scale.z));

    btVector3 localInertia(0.0f, 0.0f, 0.0f);
    if (isDynamic && mass != 0.0f) {
        collisionShape->calculateLocalInertia(mass, localInertia);
    }

    initialTransform.setIdentity();
    initialTransform.setOrigin(btVector3(position.x, position.y, position.z));
    initialTransform.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z));

    motionState = new btDefaultMotionState(initialTransform);

    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, collisionShape, localInertia);
    rigidBody = new btRigidBody(rbInfo);
    rigidBody->setUserPointer(getEntity());

    PhysicsWorld::getInstance()->getDynamicsWorld()->addRigidBody(rigidBody);
}

void PhysicsComponent::update() {
    Component::update();

    if (!rigidBody) return;

    btTransform trans;
    rigidBody->getMotionState()->getWorldTransform(trans);

    btVector3 position = trans.getOrigin();
    btQuaternion rotation = trans.getRotation();

    transform->setPosition(glm::vec3(position.x(), position.y(), position.z()));

    glm::vec3 euler;
    rotation.getEulerZYX(euler[2], euler[1], euler[0]);
    transform->setRotation(euler);
}

void PhysicsComponent::remove() {
    Component::remove();

    if (rigidBody) {
        PhysicsWorld::getInstance()->getDynamicsWorld()->removeRigidBody(rigidBody);
    }
    cleanup();
}

void PhysicsComponent::setLinearVelocity(const glm::vec3 &velocity) {
    if (rigidBody) {
        rigidBody->setLinearVelocity(btVector3(velocity.x, velocity.y, velocity.z));
    }
}

void PhysicsComponent::setAngularVelocity(const glm::vec3 &velocity) {
    if (rigidBody) {
        rigidBody->setAngularVelocity(btVector3(velocity.x, velocity.y, velocity.z));
    }
}

void PhysicsComponent::applyForce(const glm::vec3 &force, const glm::vec3 &relativePos) {
    if (rigidBody) {
        rigidBody->applyForce(
                btVector3(force.x, force.y, force.z),
                btVector3(relativePos.x, relativePos.y, relativePos.z)
        );
    }
}

void PhysicsComponent::applyImpulse(const glm::vec3 &impulse, const glm::vec3 &relativePos) {
    if (rigidBody) {
        rigidBody->applyImpulse(
                btVector3(impulse.x, impulse.y, impulse.z),
                btVector3(relativePos.x, relativePos.y, relativePos.z)
        );
    }
}

void PhysicsComponent::setFriction(float friction) {
    if (rigidBody) {
        rigidBody->setFriction(friction);
    }
}

void PhysicsComponent::setRestitution(float restitution) {
    if (rigidBody) {
        rigidBody->setRestitution(restitution);
    }
}