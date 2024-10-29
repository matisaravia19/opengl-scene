#include "window.h"

#include <utility>

Window::Window(int width, int height, std::string title) {
    this->width = width;
    this->height = height;
    this->title = std::move(title);
}

void Window::open() {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(window);

    gladLoadGL(glfwGetProcAddress);
}

void Window::swapBuffers() {
    glfwSwapBuffers(window);
}

void Window::close() {
    glfwTerminate();
}

bool Window::shouldClose() {
    return glfwWindowShouldClose(window);
}

void Window::pollEvents() {
    glfwPollEvents();
}

GLFWwindow *Window::getWindow() const {
    return window;
}
