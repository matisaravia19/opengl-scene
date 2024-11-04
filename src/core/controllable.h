#pragma once

#include "component.h"
#include "input.h"
#include "entity.h"

// Component that allows communication between Input and Camera
class Controllable : public Component {
    Input *input;
    Transform *transform; // Parent Entity transform

    double runSpeed;
    double mouseSens;
    double cameraTime;
    double bobFactor;

    void processKeyboard(long keymap, double delta_time);
    void processMouse(glm::vec2 mouse_displacement, double delta_time);
public:
    explicit Controllable(Input *input)
        : input(input)
        , transform(nullptr)
        , runSpeed(5.f)
        , mouseSens(0.5f)
        , cameraTime(0.f)
        , bobFactor(0.007f)
    {};
    void init() override;
    void update() override;
    void remove() override {};
};