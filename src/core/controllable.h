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
    double bobFactor = 0.015f;
    double interactDistance = 5.0f;

    bool isInteractPressed = false;

    void rotateCamera();
    void movePlayer();
    void interact();
public:
    void init() override;
    void update() override;
};