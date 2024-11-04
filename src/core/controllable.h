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

    void processKeyboard(long keymap, double delta_time);
    void processMouse(glm::vec2 mouse_displacement);
public:
    explicit Controllable(Input *input)
        : input(input)
        , transform(nullptr)
        , runSpeed(0.05f)
        , mouseSens(0.1f){};
    void init() override;
    void update() override;
    void remove() override {};
};