#pragma once


#include <memory>
#include "../core/component.h"
#include "animation.h"

class AnimationController : public Component {
private:
    Armature *armature;
    std::vector<Animation> animations;
    Animation *currentAnimation;
    double currentTime = 0;

public:
    void addAnimation(const Animation &animation);
    Animation *getAnimation(const std::string &name);
    void playAnimation(const std::string &name);

    void init() override;
    void update() override;
};
