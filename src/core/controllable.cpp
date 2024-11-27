#include "controllable.h"

#include "./time.h"
#include "constants.h"
#include "../physics/physicsWorld.h"
#include "../physics/physicsComponent.h"

void Controllable::init() {
    Component::init();

    input = Input::getActive();
    transform = getEntity()->getTransform();
    physicsComponent = getEntity()->getComponent<PhysicsComponent>();
}

void Controllable::update() {
    Component::update();

    rotateCamera();
    movePlayer();
    interact();
}

void Controllable::rotateCamera() {
    auto eulerAngles = transform->getEulerAngles();

    const auto mouseDisplacement = input->getMouseDisplacement();
    const auto delta = -mouseDisplacement * mouseSensitivity * Time::getDeltaTime();

    eulerAngles.x = glm::clamp(eulerAngles.x + delta.y, -PI / 2 + 0.1, PI / 2 - 0.1);
    eulerAngles.y += delta.x;
    eulerAngles.z = 0;

    transform->setEulerAngles(eulerAngles);
}

void Controllable::movePlayer() {
    auto movementInput = glm::vec2(0);
    if (input->isKeyDown(KeyCode::W)) movementInput.x += 1;
    if (input->isKeyDown(KeyCode::S)) movementInput.x -= 1;
    if (input->isKeyDown(KeyCode::A)) movementInput.y -= 1;
    if (input->isKeyDown(KeyCode::D)) movementInput.y += 1;

    movementInput *= runSpeed * Time::getDeltaTime();

    auto movement = transform->getForward() * movementInput.x + transform->getRight() * movementInput.y;
    auto currentVelocity = physicsComponent->getLinearVelocity();
    auto newVelocity = (movement == glm::vec3(0) ? movement : glm::normalize(glm::vec3(movement.x, 0, movement.z)) * 10.f) + glm::vec3(0, currentVelocity.y, 0);

    printf("Grounded: %d\n", physicsComponent->isGrounded());

    if (input->isKeyDown(KeyCode::Space) && physicsComponent->isGrounded()) {
        newVelocity.y = 5.0f;
    }

    physicsComponent->setLinearVelocity(newVelocity);
    printf("Linear velocity: %f %f %f\n", currentVelocity.x, currentVelocity.y, currentVelocity.z);

//    if (glm::dot(movement, movement) > 0) {
//        cameraTime = cameraTime + Time::getDeltaTime();
//        auto bob = sin(runSpeed * cameraTime) * bobFactor;
//        transform->translate(transform->getUp() * (float) bob);
//    } else {
//        cameraTime = 0;
//    }
}

void Controllable::interact() {
    if (input->isKeyDown(KeyCode::E) && !isInteractPressed) {
        auto position = transform->getPosition();
        auto forward = transform->getForward();

        btVector3 from(position.x, position.y, position.z);
        btVector3 to(position.x + forward.x * (float)interactDistance, position.y + forward.y * (float)interactDistance, position.z + forward.z * (float)interactDistance);

        auto rayCallback = btCollisionWorld::ClosestRayResultCallback(from, to);
        PhysicsWorld::getInstance()->getDynamicsWorld()->rayTest(from, to, rayCallback);

        if (rayCallback.hasHit()) {
            auto entity = static_cast<Entity *>(rayCallback.m_collisionObject->getUserPointer());
            if (entity) {
                auto physicsComponent = entity->getComponent<PhysicsComponent>();
                auto impulse = glm::normalize(glm::vec3(forward.x, 0, forward.z)) + glm::vec3(0, 1, 0);
                physicsComponent->setLinearVelocity(impulse * 5.0f);
            }
        } else {
            printf("Nothing to interact with\n");
        }

        isInteractPressed = true;
    } else if (!input->isKeyDown(KeyCode::E) && isInteractPressed) {
        isInteractPressed = false;
    }
}
