#include "animationController.h"

#include "../core/entity.h"
#include "../core/time.h"

void AnimationController::init() {
    Component::init();
    armature = getEntity()->getComponent<Armature>();
    currentAnimation = animations.empty() ? nullptr : &animations[0];
}

void AnimationController::addAnimation(const Animation &animation) {
    animations.push_back(animation);
}

void AnimationController::playAnimation(const std::string &name) {
    auto animation = getAnimation(name);
    if (animation) {
        currentAnimation = animation;
    }
}

Animation *AnimationController::getAnimation(const std::string &name) {
    for (auto &animation: animations) {
        if (animation.name == name) {
            return &animation;
        }
    }

    return nullptr;
}

void AnimationController::update() {
    Component::update();

    if (!currentAnimation) return;

    double currentTick = currentTime * currentAnimation->ticksPerSecond;
    for (auto &channel: currentAnimation->channels) {
        auto bone = channel.bone;

        auto position = channel.getPosition(currentTick);
        auto rotation = channel.getRotation(currentTick);
        auto scale = channel.getScale(currentTick);

        bone->setPosition(position);
        bone->setRotation(rotation);
        bone->setScale(scale);
    }

    currentTime += Time::getDeltaTime();
    if (currentTime > currentAnimation->duration / currentAnimation->ticksPerSecond) {
        currentTime = 0;
    }
}
