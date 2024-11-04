#include "transform.h"

#include <glm/ext/matrix_transform.hpp>
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

Transform::Transform(glm::vec3 position, glm::vec3 lookAt, glm::vec3 up) {
    this->position = position;
    rotation = glm::quatLookAt(glm::normalize(lookAt - position), up);
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
    return glm::degrees(glm::eulerAngles(rotation));
}

void Transform::setRotation(const glm::vec3 &rotation_in_degrees) {
    this->rotation = glm::quat(glm::radians(rotation_in_degrees));
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

void Transform::fromModelMatrix(const glm::mat4& tf_matrix) {
    // Extract position from the last column
    position = glm::vec3(tf_matrix[3]);

    // Extract scale by getting length of each axis vector
    scale.x = length(glm::vec3(tf_matrix[0]));
    scale.y = length(glm::vec3(tf_matrix[1]));
    scale.z = length(glm::vec3(tf_matrix[2]));

    // Create rotation matrix by normalizing axis vectors
    const glm::mat3 rotationMat(
        glm::vec3(tf_matrix[0]) / scale.x,  // Right vector
        glm::vec3(tf_matrix[1]) / scale.y,  // Up vector
        glm::vec3(tf_matrix[2]) / scale.z   // Forward vector
    );

    // Convert rotation matrix to quaternion
    rotation = quat_cast(rotationMat);
}

