#pragma once


#include <vector>
#include <string>
#include "../core/component.h"
#include "../core/transform.h"

class Armature;

class Bone : public Component {
private:
    Armature *armature;
    Transform *transform;
    glm::mat4 offsetMatrix;

public:
    explicit Bone(glm::mat4 offsetMatrix);

    void init() override;

    Armature *getArmature();
    void setArmature(Armature *armature);

    void setOffsetMatrix(const glm::mat4 &matrix);
    glm::mat4 getBoneMatrix();

    void setPosition(glm::vec3 position);
    void setRotation(glm::quat rotation);
    void setScale(glm::vec3 scale);
};

class Armature : public Component {
private:
    Transform *transform;
    std::vector<Bone *> bones;

public:
    void init() override;

    void addBone(Bone *bone);
    Bone *getBone(const std::string &name);
    std::vector<glm::mat4> getBoneMatrices();

    Transform *getTransform();
};
