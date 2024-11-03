#include "input.h"

#include <iostream>
#include <glm/gtc/quaternion.hpp>

#include "entity.h"

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    auto *handler = static_cast<Input*>(glfwGetWindowUserPointer(window));
    handler->processMouseMovement(xpos, ypos);
}

Input::Input(Window *window)
     : window(window)
     , deltaTime(0.f)
     , run_speed(0.03f)
     , currentTransform(1.f)
     , mouse{true, 0.f, 0.f, 0.03f}
{}

void Input::init() {
    /* Configure mouse input */
    if (glfwRawMouseMotionSupported()) {
        glfwSetInputMode(window->getWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }
    glfwSetInputMode(window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowUserPointer(window->getWindow(), this);
    glfwSetCursorPosCallback(window->getWindow(), mouseCallback);
}

void Input::poll() {
    const double current_frame = glfwGetTime();
    static double last_frame = 0.0f;
    this->deltaTime = current_frame - last_frame;

    glfwPollEvents();
}

void Input::updateTransformAngle(const glm::vec3& rotation) {
    // Create rotation quaternion from euler angles
    auto rotQuat = glm::quat(rotation);

    // Extract current position
    auto position = glm::vec3(currentTransform[3]);

    // Create new transformation matrix
    this->currentTransform = mat4_cast(rotQuat);
    this->currentTransform[3] = glm::vec4(position, 1.0f);
}

void Input::processMouseMovement(double xpos, double ypos) {
    if (mouse.first) {
        mouse.lastX = xpos;
        mouse.lastY = ypos;
        mouse.first = false;
        return;
    }

    double xoffset = (mouse.lastX - xpos) * mouse.sens;
    double yoffset = (mouse.lastY - ypos) * mouse.sens;
    mouse.lastX = xpos;
    mouse.lastY = ypos;

    // Convert current transformation to rotation angles
    glm::vec3 currentRotation = eulerAngles(quat_cast(currentTransform));

    // Update rotation angles
    currentRotation.y += static_cast<float>(glm::radians(xoffset)); // Yaw (Y-axis)
    currentRotation.x += static_cast<float>(glm::radians(yoffset)); // Pitch (X-axis)

    // Constrain pitch
    currentRotation.x = glm::clamp(currentRotation.x,
        glm::radians(-89.0f), glm::radians(89.0f));

    updateTransformAngle(currentRotation);
}

void Input::processKeyboardInput() {
    const auto window = this->window->getWindow();

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    const auto velocity = static_cast<float>(run_speed * deltaTime);

    // Extract right, up, and forward vectors from transformation matrix
    const auto right = glm::vec3(currentTransform[0]);
    const auto up = glm::vec3(currentTransform[1]);
    const auto forward = -glm::vec3(currentTransform[2]); // Negative Z is forward

    auto position = glm::vec3(currentTransform[3]);

    // Process movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        position += forward * velocity;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        position -= forward * velocity;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        position -= right * velocity;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        position += right * velocity;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        position += up * velocity;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        position -= up * velocity;

    // Update position in transformation matrix
    currentTransform[3] = glm::vec4(position, 1.0f);
}

glm::mat4 Input::getTransformMatrix() const {
    return currentTransform;
}

void Input::setTransform(const glm::mat4& tf) {
    currentTransform = tf;
};

/* Controllable methods */

void Controllable::init() {
    transform = getEntity()->getComponent<Transform>();
    input->setTransform(transform->getModelMatrix());
}

void Controllable::update() {
    input->processKeyboardInput();
    const auto tf_matrix = input->getTransformMatrix();
    transform->fromModelMatrix(tf_matrix);
}