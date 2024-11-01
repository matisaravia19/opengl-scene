#include "window.h"
#include "scene.h"

#include <utility>
#include <map>

static std::map<GLFWwindow *, Window *> windows;

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

void Window::onResize(void (*callback)(GLFWwindow *, int, int)) {
    glfwSetWindowSizeCallback(window, callback);
}

Window *Window::getActive() {
    return Scene::getActive()->getWindow();
}

glm::vec2 Window::getSize() const {
    return glm::vec2(width, height);
}

void Window::resizeCallback(GLFWwindow *glfWwindow, int width, int height) {
    auto window = windows[glfWwindow];
    window->width = width;
    window->height = height;
    glViewport(0, 0, width, height);
}

float Window::getAspectRatio() const {
    return (float) width / height;
}
