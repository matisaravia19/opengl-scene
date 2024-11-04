#include "transform.h"

#include <glm/ext/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include "constants.h"

Transform::Transform() {
    position = glm::vec3(0.0f);
    rotation = eulerAngles(glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
    scale = glm::vec3(1.0f);

    parent = nullptr;
}

Transform::Transform(glm::vec3 position, glm::quat rotation, glm::vec3 scale) {
    this->position = position;
    this->rotation = eulerAngles(rotation);
    this->scale = scale;

    parent = nullptr;
}

Transform::Transform(glm::vec3 position, glm::vec3 lookAt, glm::vec3 up) {
    this->position = position;
    rotation = eulerAngles(glm::quatLookAt(glm::normalize(lookAt - position), up));
    scale = glm::vec3(1.0f);

    parent = nullptr;
}

void Transform::addChild(Transform *child) {
    children.push_back(child);
    child->parent = this;
}

void Transform::removeChild(Transform *child) {
    children.erase(std::remove(children.begin(), children.end(), child), children.end());
    child->parent = nullptr;
}

glm::vec3 Transform::getPosition() const {
    return position;
}

void Transform::setPosition(const glm::vec3 &position) {
    this->position = position;
}

glm::vec3 Transform::getRotation() const {
    return rotation;
}

void Transform::setRotation(const glm::vec3 &rotation_in_degrees) {
    this->rotation = rotation_in_degrees;
}

glm::vec3 Transform::getScale() const {
    return scale;
}

void Transform::setScale(const glm::vec3 &scale) {
    this->scale = scale;
}

glm::mat4 Transform::getModelMatrix() {
    auto translationMatrix = glm::translate(glm::mat4(1.0f), position);
    auto rotationMatrix = glm::eulerAngleYXZ(rotation.y, rotation.x, rotation.z);
    auto scaleMatrix = glm::scale(glm::mat4(1.0f), scale);

    if (parent) {
        return parent->getModelMatrix() * translationMatrix * rotationMatrix * scaleMatrix;
    } else {
        return translationMatrix * rotationMatrix * scaleMatrix;
    }
}
