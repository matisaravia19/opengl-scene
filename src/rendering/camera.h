#pragma once


#include "../core/component.h"
#include "glm/glm.hpp"

class Camera : public Component {
private:
    glm::mat4 projection;
    glm::mat4 view;

    float fov;
    float aspect;
    float near;
    float far;

public:
    Camera(float fov, float aspect, float near, float far);
};
