#include "./time.h"

#include "GLFW/glfw3.h"

double Time::deltaTime = 1.0 / 60;
double Time::lastFrame = 0;

double Time::getDeltaTime() {
    return deltaTime;
}

void Time::endFrame() {
    double currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}
