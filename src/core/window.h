#pragma once

#include <string>
#include "glad/gl.h"
#include "GLFW/glfw3.h"

class Window {
private:
    int width;
    int height;
    std::string title;

    GLFWwindow* window = nullptr;

public:
    Window(int width, int height, std::string title);
    void open();
    void close();

    bool shouldClose();
    void pollEvents();
    void swapBuffers();

    GLFWwindow *getWindow() const;
};
