#pragma once


#include <string>
#include "armature.h"

struct AnimationChannel {
    Bone *bone;
    std::vector<std::pair<double, glm::vec3>> positions;
    std::vector<std::pair<double, glm::quat>> rotations;
    std::vector<std::pair<double, glm::vec3>> scales;

    glm::vec3 getPosition(double time) const;
    glm::quat getRotation(double time) const;
    glm::vec3 getScale(double time) const;
};

struct Animation {
    std::string name;
    double duration;
    double ticksPerSecond;
    std::vector<AnimationChannel> channels;
};
