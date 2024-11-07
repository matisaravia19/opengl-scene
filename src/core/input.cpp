#include "input.h"
#include "scene.h"


Input::Input(Window *window) {
    this->window = window;
}

void Input::init() {
    glfwSetWindowUserPointer(window->getWindow(), this);
    initKeyboard();
    initMouse();
}

void Input::initKeyboard() {
    for (int i = 0; i < GLFW_KEY_LAST; i++) {
        downKeys[i] = false;
        pressedKeys[i] = false;
        releasedKeys[i] = false;
    }

    glfwSetKeyCallback(window->getWindow(),
                       [](GLFWwindow *window, int key, int scancode, int action, int mods) -> void {
                           auto *handler = static_cast<Input *>(glfwGetWindowUserPointer(window));
                           handler->processKeyboardInput(key, action);
                       });
}

void Input::initMouse() {
    glfwSetInputMode(window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    mouseDisplacement = glm::vec2(0.0f);
    glfwGetCursorPos(window->getWindow(), &mousePosition.x, &mousePosition.x);

    glfwSetCursorPosCallback(window->getWindow(),
                             [](GLFWwindow *window, double xpos, double ypos) -> void {
                                 auto *handler = static_cast<Input *>(glfwGetWindowUserPointer(window));
                                 handler->processMouseMovement(xpos, ypos);
                             });
}

void Input::processKeyboardInput(const int key, const int action) {
    switch (action) {
        case GLFW_PRESS:
            downKeys[key] = true;
            pressedKeys[key] = true;
            break;
        case GLFW_RELEASE:
            downKeys[key] = false;
            releasedKeys[key] = true;
            break;
        default:
            break;
    }
}

void Input::processMouseMovement(const double x, const double y) {
    mouseDisplacement.x = x - mousePosition.x;
    mouseDisplacement.y = y - mousePosition.y;
}

void Input::poll() {
    clear();

    glfwPollEvents();

    if (wasKeyPressed(KeyCode::Escape)) {
        glfwSetWindowShouldClose(window->getWindow(), true);
    }
}

void Input::clear() {
    mouseDisplacement = glm::vec2(0.0f);
    glfwGetCursorPos(window->getWindow(), &mousePosition.x, &mousePosition.y);

    for (int i = 0; i < GLFW_KEY_LAST; i++) {
        pressedKeys[i] = false;
        releasedKeys[i] = false;
    }
}

bool Input::isKeyDown(KeyCode key) const {
    return downKeys[key];
}

bool Input::wasKeyPressed(KeyCode key) const {
    return pressedKeys[key];
}

bool Input::wasKeyReleased(KeyCode key) const {
    return releasedKeys[key];
}

glm::vec<2, double> Input::getMouseDisplacement() {
    return mouseDisplacement;
}

Input *Input::getActive() {
    return Scene::getActive()->getInput();
}
