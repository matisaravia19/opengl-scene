#pragma once


#include <glm/mat4x4.hpp>

#include "component.h"
#include "transform.h"
#include "window.h"

enum KeyCode {
    A = GLFW_KEY_A,
    B = GLFW_KEY_B,
    C = GLFW_KEY_C,
    D = GLFW_KEY_D,
    E = GLFW_KEY_E,
    F = GLFW_KEY_F,
    G = GLFW_KEY_G,
    H = GLFW_KEY_H,
    I = GLFW_KEY_I,
    J = GLFW_KEY_J,
    K = GLFW_KEY_K,
    L = GLFW_KEY_L,
    M = GLFW_KEY_M,
    N = GLFW_KEY_N,
    O = GLFW_KEY_O,
    P = GLFW_KEY_P,
    Q = GLFW_KEY_Q,
    R = GLFW_KEY_R,
    S = GLFW_KEY_S,
    T = GLFW_KEY_T,
    U = GLFW_KEY_U,
    V = GLFW_KEY_V,
    W = GLFW_KEY_W,
    X = GLFW_KEY_X,
    Y = GLFW_KEY_Y,
    Z = GLFW_KEY_Z,
    Space = GLFW_KEY_SPACE,
    Enter = GLFW_KEY_ENTER,
    Escape = GLFW_KEY_ESCAPE,
    Left = GLFW_KEY_LEFT,
    Right = GLFW_KEY_RIGHT,
    Up = GLFW_KEY_UP,
    Down = GLFW_KEY_DOWN
};

class Input {
private:
    Window *window;

    bool downKeys[GLFW_KEY_LAST];
    bool pressedKeys[GLFW_KEY_LAST];
    bool releasedKeys[GLFW_KEY_LAST];

    glm::vec<2, double> mousePosition;
    glm::vec<2, double> mouseDisplacement;

    void clear();

    void initKeyboard();
    void initMouse();

    void processMouseMovement(double x, double y);
    void processKeyboardInput(int key, int action);

public:
    explicit Input(Window *window);

    void init();
    void poll();

    bool isKeyDown(KeyCode key) const;
    bool wasKeyPressed(KeyCode key) const;
    bool wasKeyReleased(KeyCode key) const;
    glm::vec<2, double> getMouseDisplacement();
    void setCursorVisibility(bool visible);

    static Input *getActive();
};