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
    glm::mat4 clip = projection * view;

    // Extract the numbers for the RIGHT plane
    frustum.planes[0].normal.x = clip[0][3] - clip[0][0];
    frustum.planes[0].normal.y = clip[1][3] - clip[1][0];
    frustum.planes[0].normal.z = clip[2][3] - clip[2][0];
    frustum.planes[0].distance = clip[3][3] - clip[3][0];

    // Extract the numbers for the LEFT plane
    frustum.planes[1].normal.x = clip[0][3] + clip[0][0];
    frustum.planes[1].normal.y = clip[1][3] + clip[1][0];
    frustum.planes[1].normal.z = clip[2][3] + clip[2][0];
    frustum.planes[1].distance = clip[3][3] + clip[3][0];

    // Extract the BOTTOM plane
    frustum.planes[2].normal.x = clip[0][3] + clip[0][1];
    frustum.planes[2].normal.y = clip[1][3] + clip[1][1];
    frustum.planes[2].normal.z = clip[2][3] + clip[2][1];
    frustum.planes[2].distance = clip[3][3] + clip[3][1];

    // Extract the TOP plane
    frustum.planes[3].normal.x = clip[0][3] - clip[0][1];
    frustum.planes[3].normal.y = clip[1][3] - clip[1][1];
    frustum.planes[3].normal.z = clip[2][3] - clip[2][1];
    frustum.planes[3].distance = clip[3][3] - clip[3][1];

    // Extract the FAR plane
    frustum.planes[4].normal.x = clip[0][3] - clip[0][2];
    frustum.planes[4].normal.y = clip[1][3] - clip[1][2];
    frustum.planes[4].normal.z = clip[2][3] - clip[2][2];
    frustum.planes[4].distance = clip[3][3] - clip[3][2];

    // Extract the NEAR plane
    frustum.planes[5].normal.x = clip[0][3] + clip[0][2];
    frustum.planes[5].normal.y = clip[1][3] + clip[1][2];
    frustum.planes[5].normal.z = clip[2][3] + clip[2][2];
    frustum.planes[5].distance = clip[3][3] + clip[3][2];

    for (auto &plane: frustum.planes) {
        float length = glm::length(plane.normal);
        plane.normal /= length;
        plane.distance /= length;
    }
}

glm::vec3 Camera::getPosition() const {
    return transform->getPosition();
}
