#include "input.h"

#include <iostream>
#include <glm/gtc/quaternion.hpp>

#include "entity.h"

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    auto *handler = static_cast<Input*>(glfwGetWindowUserPointer(window));
    handler->processMouseMovement(xpos, ypos);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto *handler = static_cast<Input*>(glfwGetWindowUserPointer(window));
    handler->processKeyboardInput(key, scancode, action, mods);
}

Input::Input(Window *window)
    : window(window)
    , deltaTime(0.f)
    , pressedKeys(0.f)
    , mouseDisplacement(0.f)
    , mouse{true, 0.f, 0.f}
{}

void Input::init() {
    /* Configure mouse input */
    if (glfwRawMouseMotionSupported()) {
        glfwSetInputMode(window->getWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }
    glfwSetInputMode(window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowUserPointer(window->getWindow(), this);
    glfwSetCursorPosCallback(window->getWindow(), mouseCallback);

    /* Configure KB input */
    glfwSetKeyCallback(window->getWindow(), keyCallback);
}

void Input::poll() {
    const double current_frame = glfwGetTime();
    static double last_frame = 0.0f;
    this->deltaTime = current_frame - last_frame;
    last_frame = current_frame;

    glfwPollEvents();
}

void Input::processKeyboardInput(const int key, int _scancode, const int action, int _mods) {
    if (const auto window = this->window->getWindow();
        glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    // Process movement keys down
    if (key ==  GLFW_KEY_W && action == GLFW_PRESS)
        pressedKeys |= KeyW;
    if (key ==  GLFW_KEY_A && action == GLFW_PRESS)
        pressedKeys |= KeyA;
    if (key ==  GLFW_KEY_S && action == GLFW_PRESS)
        pressedKeys |= KeyS;
    if (key ==  GLFW_KEY_D && action == GLFW_PRESS)
        pressedKeys |= KeyD;

    // Process movement keys up
    if (key ==  GLFW_KEY_W && action == GLFW_RELEASE)
        pressedKeys &= ~KeyW;
    if (key ==  GLFW_KEY_A && action == GLFW_RELEASE)
        pressedKeys &= ~KeyA;
    if (key ==  GLFW_KEY_S && action == GLFW_RELEASE)
        pressedKeys &= ~KeyS;
    if (key ==  GLFW_KEY_D && action == GLFW_RELEASE)
        pressedKeys &= ~KeyD;
}

void Input::processMouseMovement(const double xpos, const double ypos) {
    if (mouse.first) {
        mouse.lastX = xpos;
        mouse.lastY = ypos;
        mouse.first = false;
        return;
    }

    const double xoffset = xpos - mouse.lastX;
    const double yoffset = ypos - mouse.lastY;
    mouse.lastX = xpos;
    mouse.lastY = ypos;

    std::cout << "offset: " << xoffset << ' ' << yoffset << '\n';


    mouseDisplacement = glm::vec2(xoffset, yoffset);
}

long Input::getKeysDown() const {
    return pressedKeys;
}

glm::vec2 Input::consumeMouseDisplacement() {
    const auto md = mouseDisplacement;
    mouseDisplacement = glm::vec2(0.f, 0.f);
    return md;
}

double Input::getDeltaTime() const {
    return deltaTime;
}
