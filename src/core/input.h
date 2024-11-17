#pragma once


#include <glm/mat4x4.hpp>

#include "component.h"
#include "transform.h"
#include "window.h"

enum Keycodes {
    KeyW = 1,
    KeyA = 2,
    KeyS = 4,
    KeyD = 8,
    MovementMask = 15,
    KeyEsc = 16,
};

class Input {
private:
    Window *window;

    double deltaTime;
    bool paused;

    long pressedKeys;
    glm::vec2 mouseDisplacement;
    struct {
        bool first;
        double lastX;
        double lastY;
    } mouse;

    void updateTransformAngle(const glm::vec3& rotation);
public:
    explicit Input(Window *window);
    void init();
    void poll();
    void togglePause();

    [[nodiscard]] long getKeysDown() const;
    [[nodiscard]] glm::vec2 consumeMouseDisplacement();
    [[nodiscard]] double getDeltaTime() const;
    void processMouseMovement(double xpos, double ypos);
    void processKeyboardInput(int key, int _scancode, int action, int _mods);
};