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

    glm::vec3 boundingBox = meshRenderer ? meshRenderer->getMesh()->bounds : glm::vec3(1.0f);
    if (hitboxType == 0) {
        collisionShape = new btBoxShape(btVector3(boundingBox.x * scale.x, boundingBox.y * scale.y, boundingBox.z * scale.z));
    } else if (hitboxType == 1) {
        auto maxVertexFromBoundingBox = glm::max(glm::max(boundingBox.x * scale.x, boundingBox.y * scale.y), boundingBox.z * scale.z);
        collisionShape = new btSphereShape(maxVertexFromBoundingBox);
    } else if (hitboxType == 2) {
        collisionShape = new btCapsuleShape(1, 2);
    } else if (hitboxType == 3) {
        auto mesh = meshRenderer->getMesh();
        auto vertices = mesh->vertices;
        auto indices = mesh->indices;
        auto scale = transform->getScale();
        auto transform = getEntity()->getTransform();
        auto matrix = transform->getModelMatrix();

        btTriangleMesh *triangleMesh = new btTriangleMesh();
        for (int i = 0; i < indices.size(); i += 3) {
            auto vertex0 = matrix * glm::vec4(vertices[indices[i]].position, 1.0f);
            auto vertex1 = matrix * glm::vec4(vertices[indices[i + 1]].position, 1.0f);
            auto vertex2 = matrix * glm::vec4(vertices[indices[i + 2]].position, 1.0f);
            btVector3 btVertex0(vertex0.x, vertex0.y, vertex0.z);
            btVector3 btVertex1(vertex1.x, vertex1.y, vertex1.z);
            btVector3 btVertex2(vertex2.x, vertex2.y, vertex2.z);
            triangleMesh->addTriangle(btVertex0, btVertex1, btVertex2);
        }

        collisionShape = new btBvhTriangleMeshShape(triangleMesh, true);
    } else if (hitboxType == 4) { // tree
        collisionShape = new btCapsuleShape(0.3, 5);
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
        rigidBody->setAngularFactor(btVector3(0, 0, 0));
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
    btVector3 rayEnd = playerPosition - btVector3(0, 2.1f, 0);

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