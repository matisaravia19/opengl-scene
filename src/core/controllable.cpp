// ReSharper disable CppMemberFunctionMayBeConst
#include "controllable.h"

#include <iostream>


void Controllable::init() {
    transform = getEntity()->getComponent<Transform>();
}

void Controllable::update() {
    const auto key_map = input->getKeysDown();
    const auto mouse_displacement = input->consumeMouseDisplacement();
    const auto delta_time = input->getDeltaTime();

    processKeyboard(key_map, delta_time);
    processMouse(mouse_displacement);
}

void Controllable::processMouse(glm::vec2 mouse_displacement) {
    glm::vec3 currentRotation = transform->getRotation();

    mouse_displacement *= mouseSens;
    // Update rotation angles
    currentRotation.y += mouse_displacement.x; // Yaw (Y-axis)
    currentRotation.x += mouse_displacement.y; // Pitch (X-axis)

    //Constrain pitch
    currentRotation.x = glm::clamp(currentRotation.x, -89.f, 89.f);

    transform->setRotation(currentRotation);
}

void Controllable::processKeyboard(const long keymap, const double delta_time) {
    const auto velocity = static_cast<float>(runSpeed * delta_time);

    // Extract right, up, and forward vectors from transformation matrix
    const auto currentTransform = transform->getModelMatrix();
    const auto right = glm::vec3(currentTransform[0]);
    const auto up = glm::vec3(currentTransform[1]);
    const auto forward = -glm::vec3(currentTransform[2]); // Negative Z is forward

    auto position = glm::vec3(currentTransform[3]);

    if (keymap & KeyW)
        position += forward * velocity;
    if (keymap & KeyS)
        position -= forward * velocity;
    if (keymap & KeyA)
        position -= right * velocity;
    if (keymap & KeyD)
        position += right * velocity;

    // Add view bobbing
    cameraTime = keymap ? cameraTime + delta_time : 0.f;
    const double offset_factor = sin(runSpeed * cameraTime) * bobFactor;
    position += up * static_cast<float>(offset_factor);

    // Note: This could be improved by slowly lowering the camera if
    // a movement key is released while the camera isn't at its lowest
    // offset.

    transform->setPosition(position);
}
