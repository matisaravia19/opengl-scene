#include "controllable.h"

#include "./time.h"
#include "constants.h"

void Controllable::init() {
    input = Input::getActive();
    transform = getEntity()->getTransform();
}

void Controllable::update() {
    rotateCamera();
    movePlayer();
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
    transform->translate(movement);

    if (glm::dot(movement, movement) > 0) {
        cameraTime = cameraTime + Time::getDeltaTime();
        auto bob = sin(runSpeed * cameraTime) * bobFactor;
        transform->translate(transform->getUp() * (float) bob);
    } else {
        cameraTime = 0;
    }
}
