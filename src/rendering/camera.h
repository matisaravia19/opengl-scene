#pragma once


#include "glm/glm.hpp"
#include "../core/component.h"
#include "../core/transform.h"

class Camera : public Component {
private:
    glm::mat4 projection;
    Transform *transform;

    float fov;
    float aspect;
    float near;
    float far;

public:
    Camera(float fov, float aspect, float near, float far);

    void init() override;

    glm::mat4 getProjection() const;
    glm::mat4 getView() const;
};
