#pragma once

#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "component.h"

class Transform : public Component {
private:
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 eulerAngles;
    glm::vec3 scale;

    Transform *parent;
    std::vector<Transform *> children;

public:
    Transform();
    Transform(glm::vec3 position, glm::quat rotation, glm::vec3 scale);
    Transform(glm::vec3 position, glm::vec3 lookAt, glm::vec3 up);

    void addChild(Transform *child);
    void removeChild(Transform *child);

    glm::vec3 getPosition() const;
    void setPosition(const glm::vec3 &position);
    glm::quat getRotation() const;
    void setRotation(const glm::quat &rotation);
    glm::vec3 getEulerAngles() const;
    void setEulerAngles(const glm::vec3 &eulerAngles);
    glm::vec3 getScale() const;
    void setScale(const glm::vec3 &scale);

    glm::vec3 getWorldPosition() const;

    glm::vec3 getForward();
    glm::vec3 getRight();
    glm::vec3 getUp();

    glm::mat4 getModelMatrix() const;
    glm::mat4 getLocalModelMatrix() const;
    glm::mat4 getModelMatrixRelativeTo(Transform *relativeTo) const;

    void translate(const glm::vec3 &translation);
    void rotate(const glm::vec3 &eulerAngles);

    Transform *getParent();
};
