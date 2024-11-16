#pragma once

#include <string>

#include "settings.h"
#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "glm/vec2.hpp"

class Window : SettingsObserver {
private:
    int width;
    int height;
    std::string title;

    GLFWwindow *window = nullptr;

    static void resizeCallback(GLFWwindow *glfWwindow, int width, int height);

public:
    Window(int width, int height, std::string title);

    void open();
    void close();
    bool shouldClose();

    void pollEvents();
    void swapBuffers();
    void onResize(void (*callback)(GLFWwindow *, int, int));

    static Window *getActive();
    GLFWwindow *getWindow() const;
    glm::vec2 getSize() const;
    float getAspectRatio() const;

    void notify(Settings* settings) override;
};
