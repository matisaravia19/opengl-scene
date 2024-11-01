#include "camera.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "../core/entity.h"
#include "renderer.h"

Camera::Camera(float fov, float near, float far) {
    this->fov = fov;
    this->near = near;
    this->far = far;
}

void Camera::init() {
    transform = getEntity()->getComponent<Transform>();
    Renderer::getActive()->setCamera(this);

    aspect = Window::getActive()->getAspectRatio();
    updateProjection();
}

void Camera::update() {
    auto aspect = Window::getActive()->getAspectRatio();
    if (aspect != this->aspect) {
        this->aspect = aspect;
        updateProjection();
    }
}

glm::mat4 Camera::getProjection() const {
    return projection;
}

glm::mat4 Camera::getView() const {
    return glm::inverse(transform->getModelMatrix());
}

void Camera::updateProjection() {
    projection = glm::perspective(fov, aspect, near, far);
}
