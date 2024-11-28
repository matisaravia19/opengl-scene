#include "window.h"

#include <iostream>

#include "scene.h"

#include <utility>
#include <map>

static std::map<GLFWwindow *, Window *> windows;

Window::Window(int width, int height, std::string title) {
    this->width = width;
    this->height = height;
    this->title = std::move(title);
    this->fullscreen = false;
}

void Window::open() {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    windows[window] = this;
    glfwMakeContextCurrent(window);

    glfwSetWindowSizeCallback(window, resizeCallback);

    gladLoadGL(glfwGetProcAddress);
}

void Window::swapBuffers() {
    glfwSwapBuffers(window);
}

void Window::close() {
    glfwTerminate();
    windows.erase(window);
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

Window *Window::getActive() {
    return Scene::getActive()->getWindow();
}

glm::vec2 Window::getSize() const {
    return glm::vec2(width, height);
}

void Window::setFullscreen(bool fullscreen) {
    if (fullscreen == this->fullscreen) {
        return;
    }

    this->fullscreen = fullscreen;
    if (fullscreen) {
        const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, GLFW_DONT_CARE);
    } else {
        glfwSetWindowMonitor(window, nullptr, 0, 50, width, height, GLFW_DONT_CARE);
    }
}

void Window::resizeCallback(GLFWwindow *glfWwindow, int width, int height) {
    auto window = windows[glfWwindow];
    window->width = width;
    window->height = height;

    Renderer::getActive()->onWindowResize(width, height);
}

float Window::getAspectRatio() const {
    return (float) width / height;
}

int Window::getWidth() const {
    return width;
}

int Window::getHeight() const {
    return height;
}
