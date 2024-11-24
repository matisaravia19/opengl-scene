#include "armature.h"

#include "../core/entity.h"

Bone::Bone(glm::mat4 offsetMatrix) {
    this->offsetMatrix = offsetMatrix;
}

void Bone::init() {
    Component::init();
    transform = getEntity()->getTransform();
}

Armature *Bone::getArmature() {
    return armature;
}

void Bone::setArmature(Armature *armature) {
    this->armature = armature;
}

void Bone::setOffsetMatrix(const glm::mat4 &matrix) {
    offsetMatrix = matrix;
}

glm::mat4 Bone::getBoneMatrix() {
    return transform->getModelMatrix() * offsetMatrix;
}

void Bone::setPosition(glm::vec3 position) {
    transform->setPosition(position);
}

void Bone::setRotation(glm::quat rotation) {
    transform->setRotation(rotation);
}

void Bone::setScale(glm::vec3 scale) {
    transform->setScale(scale);
}

void Armature::init() {
    Component::init();
    transform = getEntity()->getTransform();
}

void Armature::addBone(Bone *bone) {
    bones.push_back(bone);
    bone->setArmature(this);
}

Bone *Armature::getBone(const std::string &name) {
    for (auto bone: bones) {
        if (bone->getEntity()->getName() == name) {
            return bone;
        }
    }
    return nullptr;
}

std::vector<glm::mat4> Armature::getBoneMatrices() {
    auto matrices = std::vector<glm::mat4>(bones.size());
    for (int i = 0; i < bones.size(); i++) {
//        auto boneRelativeMatrix = bone->getModelMatrixRelativeTo(transform);
//        auto offsetMatrix =
//        matrices.push_back(bone->getModelMatrixRelativeTo(transform));
        matrices[i] = bones[i]->getBoneMatrix();
    }
    return matrices;
}

Transform *Armature::getTransform() {
    return transform;
}
