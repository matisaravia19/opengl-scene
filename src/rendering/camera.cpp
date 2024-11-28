#include "camera.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "../core/entity.h"
#include "renderer.h"

Camera::Camera(float fov, float near, float far) {
    this->fov = fov;
    this->near = near;
    this->far = 1000.0f;
}

void Camera::init() {
    Component::init();

    transform = getEntity()->getComponent<Transform>();
    Renderer::getActive()->setCamera(this);

    aspect = Window::getActive()->getAspectRatio();
    updateProjection();
}

void Camera::update() {
    Component::update();

    auto aspect = Window::getActive()->getAspectRatio();
    if (aspect != this->aspect) {
        this->aspect = aspect;
        updateProjection();
    }

    updateView();
    updateFrustum();
}

glm::mat4 Camera::getProjection() const {
    return projection;
}

glm::mat4 Camera::getView() const {
    return view;
}

Frustum Camera::getFrustum() const {
    return frustum;
}

void Camera::updateProjection() {
    projection = glm::perspective(fov, aspect, near, far);
}

void Camera::updateView() {
    view = glm::inverse(transform->getModelMatrix());
}

void Camera::updateFrustum() {
    frustum = Frustum(projection * view);
}

glm::vec3 Camera::getPosition() const {
    return transform->getPosition();
}
