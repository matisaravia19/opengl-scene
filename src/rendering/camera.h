#pragma once


#include "glm/glm.hpp"
#include "../core/component.h"
#include "../core/transform.h"
#include "../core/volumes.h"

class Camera : public Component {
private:
    glm::mat4 projection;
    glm::mat4 view;
    Frustum frustum;
    Transform *transform;

    float fov;
    float aspect;
    float near;
    float far;

    void updateProjection();
    void updateView();
    void updateFrustum();

public:
    Camera(float fov, float near, float far);

    void init() override;
    void update() override;

    glm::mat4 getProjection() const;
    glm::mat4 getView() const;
    Frustum getFrustum() const;
    glm::vec3 getPosition() const;
};
