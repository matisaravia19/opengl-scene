#include "input.h"


void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

Input::Input(Window *window) {
    this->window = window;
}

void Input::init() {
    glfwSetKeyCallback(window->getWindow(), keyCallback);
}

void Input::poll() {
    glfwPollEvents();
}
