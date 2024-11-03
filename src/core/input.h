#pragma once


#include <glm/mat4x4.hpp>

#include "component.h"
#include "transform.h"
#include "window.h"

class Input {
private:
    Window *window;

    double deltaTime;
    double run_speed;

    glm::mat4 currentTransform;

    struct {
        bool first;
        double lastX;
        double lastY;
        double sens;
    } mouse;

    void updateTransformAngle(const glm::vec3& rotation);
public:
    explicit Input(Window *window);
    void init();
    void poll();
    void setTransform(const glm::mat4& tf);
    void processMouseMovement(double xpos, double ypos);

    [[nodiscard]] glm::mat4 getTransformMatrix() const;
    void processKeyboardInput();
};

// Component that allows communication between Input and Camera
class Controllable : public Component {
    Input *input;
    Transform *transform; // Parent Entity transform

public:
    explicit Controllable(Input *input) : input(input), transform(nullptr) {};
    void init() override;
    void update() override;
    void remove() override {};
};