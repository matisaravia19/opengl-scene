#include "camera.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "../core/entity.h"

Camera::Camera(float fov, float aspect, float near, float far) {
    this->fov = fov;
    this->aspect = aspect;
    this->near = near;
    this->far = far;
    this->projection = glm::perspective(glm::radians(fov), aspect, near, far);
}

void Camera::init() {
    transform = getEntity()->getComponent<Transform>();
}

glm::mat4 Camera::getProjection() const {
    return projection;
}

glm::mat4 Camera::getView() const {
    return glm::inverse(transform->getModelMatrix());
}
