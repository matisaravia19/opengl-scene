#include "physicsComponent.h"
#include "physicsWorld.h"
#include "glm/gtc/type_ptr.hpp"
#include "../rendering/meshRenderer.h"
#include "../core/controllable.h"


PhysicsComponent::PhysicsComponent(float mass, bool isDynamic, int hitboxType)
        : mass(mass), hitboxType(hitboxType), isDynamic(isDynamic), rigidBody(nullptr), collisionShape(nullptr), motionState(nullptr) {
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
    auto meshRenderer = getEntity()->getComponent<MeshRenderer>();

    glm::vec3 position = transform->getPosition();
    glm::vec3 scale = transform->getScale();
    glm::vec3 rotation = transform->getEulerAngles();

    glm::vec3 boundingBox = meshRenderer ? meshRenderer->getMesh()->getBoundingBox() : glm::vec3(1.0f);
    if (hitboxType == 0) {
        collisionShape = new btBoxShape(btVector3(boundingBox.x * scale.x, boundingBox.y * scale.y, boundingBox.z * scale.z));
    } else if (hitboxType == 1) {
        auto maxVertexFromBoundingBox = glm::max(glm::max(boundingBox.x * scale.x, boundingBox.y * scale.y), boundingBox.z * scale.z);
        collisionShape = new btSphereShape(maxVertexFromBoundingBox);
    } else if (hitboxType == 2) {
        collisionShape = new btBoxShape(btVector3(1, 1, 1));
    }

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

    if (getEntity()->getComponent<Controllable>()) {
        rigidBody->setActivationState(DISABLE_DEACTIVATION);
    }

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
    if (hitboxType != 2) {
        transform->setRotation(euler);
    }
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
        rigidBody->activate();
        rigidBody->setLinearVelocity(btVector3(velocity.x, velocity.y, velocity.z));
    }
}

void PhysicsComponent::setAngularVelocity(const glm::vec3 &velocity) {
    if (rigidBody) {
        rigidBody->activate();
        rigidBody->setAngularVelocity(btVector3(velocity.x, velocity.y, velocity.z));
    }
}

void PhysicsComponent::applyForce(const glm::vec3 &force, const glm::vec3 &relativePos) {
    if (rigidBody) {
        rigidBody->activate();
        rigidBody->applyForce(
                btVector3(force.x, force.y, force.z),
                btVector3(relativePos.x, relativePos.y, relativePos.z)
        );
    }
}

void PhysicsComponent::applyImpulse(const glm::vec3 &impulse, const glm::vec3 &relativePos) {
    if (rigidBody) {
        rigidBody->activate();
        rigidBody->applyImpulse(
                btVector3(impulse.x, impulse.y, impulse.z),
                btVector3(relativePos.x, relativePos.y, relativePos.z)
        );
    }
}

bool PhysicsComponent::isGrounded() {
    if (!rigidBody) return false;

    btTransform transform;
    rigidBody->getMotionState()->getWorldTransform(transform);
    btVector3 playerPosition = transform.getOrigin();

    btVector3 rayStart = playerPosition;
    btVector3 rayEnd = playerPosition - btVector3(0, 1.1f, 0); // Adjust 1.1f for player's height and a small offset

    btCollisionWorld::ClosestRayResultCallback rayCallback(rayStart, rayEnd);
    PhysicsWorld::getInstance()->getDynamicsWorld()->rayTest(rayStart, rayEnd, rayCallback);

    if (rayCallback.hasHit()) {
        btRigidBody *hitBody = (btRigidBody *)btRigidBody::upcast(rayCallback.m_collisionObject);
        if (hitBody && hitBody != rigidBody) {
            return true;
        }
    }

    return false;
}

void PhysicsComponent::setFriction(float friction) {
    if (rigidBody) {
        rigidBody->activate();
        rigidBody->setFriction(friction);
    }
}

void PhysicsComponent::setRestitution(float restitution) {
    if (rigidBody) {
        rigidBody->activate();
        rigidBody->setRestitution(restitution);
    }
}

glm::vec3 PhysicsComponent::getLinearVelocity() {
    if (rigidBody) {
        btVector3 velocity = rigidBody->getLinearVelocity();
        return glm::vec3(velocity.x(), velocity.y(), velocity.z());
    }
    return glm::vec3(0);
}