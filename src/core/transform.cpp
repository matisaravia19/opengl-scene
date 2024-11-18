#include "transform.h"

#include <glm/ext/matrix_transform.hpp>
#include "constants.h"

Transform::Transform() {
    position = glm::vec3(0.0f);
    rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    eulerAngles = glm::vec3(0.0f);
    scale = glm::vec3(1.0f);

    parent = nullptr;
}

Transform::Transform(glm::vec3 position, glm::quat rotation, glm::vec3 scale) {
    this->position = position;
    this->rotation = rotation;
    this->eulerAngles = glm::eulerAngles(rotation);
    this->scale = scale;

    parent = nullptr;
}

Transform::Transform(glm::vec3 position, glm::vec3 lookAt, glm::vec3 up) {
    this->position = position;
    this->rotation = glm::quatLookAt(glm::normalize(lookAt - position), up);
    this->eulerAngles = glm::eulerAngles(rotation);
    this->scale = glm::vec3(1.0f);

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

glm::quat Transform::getRotation() const {
    return rotation;
}

void Transform::setRotation(const glm::quat &rotation) {
    this->rotation = rotation;
    this->eulerAngles = glm::eulerAngles(rotation);
}

glm::vec3 Transform::getEulerAngles() const {
    return eulerAngles;
}

void Transform::setEulerAngles(const glm::vec3 &eulerAngles) {
    this->eulerAngles = eulerAngles;
    this->rotation = glm::quat(eulerAngles);
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

void Transform::translate(const glm::vec3 &translation) {
    position += translation;
}

void Transform::rotate(const glm::vec3 &eulerAngles) {
    this->eulerAngles += eulerAngles;
    this->rotation = glm::quat(this->eulerAngles);
}

glm::vec3 Transform::getForward() {
    return glm::normalize(glm::vec3(getModelMatrix() * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)));
}

glm::vec3 Transform::getRight() {
    return glm::normalize(glm::vec3(getModelMatrix() * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)));
}

glm::vec3 Transform::getUp() {
    return glm::normalize(glm::vec3(getModelMatrix() * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));
}
