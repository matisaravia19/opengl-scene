#include "transform.h"

#include "glm/ext/matrix_transform.hpp"
#include "constants.h"

Transform::Transform() {
    position = glm::vec3(0.0f);
    rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    scale = glm::vec3(1.0f);

    parent = nullptr;
}

Transform::Transform(glm::vec3 position, glm::quat rotation, glm::vec3 scale) {
    this->position = position;
    this->rotation = rotation;
    this->scale = scale;

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
    return glm::degrees(glm::eulerAngles(rotation));
}

void Transform::setRotation(const glm::vec3 &rotation) {
    this->rotation = glm::quat(glm::radians(rotation));
}

glm::vec3 Transform::getScale() const {
    return scale;
}

void Transform::setScale(const glm::vec3 &scale) {
    this->scale = scale;
}

glm::mat4 Transform::getModelMatrix() {
    auto translationMatrix = glm::translate(glm::mat4(1.0f), position);
    auto rotationMatrix = glm::mat4_cast(rotation);
    auto scaleMatrix = glm::scale(glm::mat4(1.0f), scale);

    if (parent) {
        return parent->getModelMatrix() * translationMatrix * rotationMatrix * scaleMatrix;
    } else {
        return translationMatrix * rotationMatrix * scaleMatrix;
    }
}
