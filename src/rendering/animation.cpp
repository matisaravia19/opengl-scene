#include "animation.h"

template<typename T, typename Mix>
static T getInterpolatedValue(double time, const std::vector<std::pair<double, T>> &values, Mix mix) {
    if (values.size() == 1) {
        return values[0].second;
    }

    if (time <= values[0].first) {
        return values[0].second;
    }

    if (time >= values[values.size() - 1].first) {
        return values[values.size() - 1].second;
    }

    for (int i = 0; i < values.size() - 1; i++) {
        if (time >= values[i].first && time <= values[i + 1].first) {
            float t = (time - values[i].first) / (values[i + 1].first - values[i].first);
            return mix(values[i].second, values[i + 1].second, t);
        }
    }

    return T();
}

static glm::vec3 getInterpolatedValue(double time, const std::vector<std::pair<double, glm::vec3>> &values) {
    return getInterpolatedValue(time, values, [](const glm::vec3 &a, const glm::vec3 &b, float t) {
        return glm::mix(a, b, t);
    });
}

static glm::quat getInterpolatedValue(double time, const std::vector<std::pair<double, glm::quat>> &values) {
    return getInterpolatedValue(time, values, [](const glm::quat &a, const glm::quat &b, float t) {
        return glm::slerp(a, b, t);
    });
}

glm::vec3 AnimationChannel::getPosition(double time) const {
    return getInterpolatedValue(time, positions);
}

glm::quat AnimationChannel::getRotation(double time) const {
    return getInterpolatedValue(time, rotations);
}

glm::vec3 AnimationChannel::getScale(double time) const {
    return getInterpolatedValue(time, scales);
}