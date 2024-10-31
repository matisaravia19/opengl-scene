#include "camera.h"
#include "glm/ext/matrix_clip_space.hpp"

Camera::Camera(float fov, float aspect, float near, float far) {
    this->fov = fov;
    this->aspect = aspect;
    this->near = near;
    this->far = far;
    this->projection = glm::perspective(glm::radians(fov), aspect, near, far);
}
