#include "PhysicsComponent.h"
#include "PhysicsWorld.h"
#include <glm/gtc/type_ptr.hpp>
#include "../rendering/MeshRenderer.h"
#include "controllable.h"


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
        collisionShape = new btCapsuleShape(2, 3);
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
        rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
        rigidBody->setActivationState(DISABLE_DEACTIVATION);
    }

    PhysicsWorld::getInstance()->getDynamicsWorld()->addRigidBody(rigidBody);
}

void PhysicsComponent::update() {
    Component::update();

    if (!rigidBody) return;

    if (rigidBody->isKinematicObject()) {
        glm::vec3 position = transform->getPosition();
        glm::vec3 rotation = transform->getEulerAngles();

        btTransform newTransform;
        newTransform.setIdentity();
        newTransform.setOrigin(btVector3(position.x, position.y, position.z));
        newTransform.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z));

        rigidBody->setWorldTransform(newTransform);
        rigidBody->getMotionState()->setWorldTransform(newTransform);

        rigidBody->setLinearVelocity(btVector3(0, 0, 0));
        rigidBody->setAngularVelocity(btVector3(0, 0, 0));
    } else {
        btTransform trans;
        rigidBody->getMotionState()->getWorldTransform(trans);

        btVector3 position = trans.getOrigin();
        btQuaternion rotation = trans.getRotation();

        transform->setPosition(glm::vec3(position.x(), position.y(), position.z()));

        glm::vec3 euler;
        rotation.getEulerZYX(euler[2], euler[1], euler[0]);
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