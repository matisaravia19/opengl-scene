#pragma once

#include <string>

#include "settings.h"
#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "glm/vec2.hpp"

class Window : public SettingsObserver {
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

    static Window *getActive();
    GLFWwindow *getWindow() const;
    glm::vec2 getSize() const;
    int getWidth() const;
    int getHeight() const;
    float getAspectRatio() const;

    void notify(Settings *settings) override;
};
