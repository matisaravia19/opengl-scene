#pragma once


#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "component.h"

class Transform : public Component {
private:
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;

    Transform *parent;
    std::vector<Transform *> children;

public:
    Transform();
    Transform(glm::vec3 position, glm::quat rotation, glm::vec3 scale);

    void addChild(Transform *child);
    void removeChild(Transform *child);

    glm::vec3 getPosition() const;
    void setPosition(const glm::vec3 &position);
    glm::vec3 getRotation() const;
    void setRotation(const glm::vec3 &rotation);
    glm::vec3 getScale() const;
    void setScale(const glm::vec3 &scale);

    glm::mat4 getModelMatrix();
};
