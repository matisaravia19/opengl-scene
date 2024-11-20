#pragma once

#include "component.h"
#include "input.h"
#include "entity.h"

class Controllable : public Component {
    Input *input;
    Transform *transform;

    double runSpeed = 10.0f;
    double mouseSensitivity = 0.5f;
    double cameraTime;
    double bobFactor = 0.001f;

    void rotateCamera();
    void movePlayer();
public:
    void init() override;
    void update() override;
};