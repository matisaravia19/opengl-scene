#pragma once


#include "glm/glm.hpp"
#include "../core/component.h"

class Light : public Component {
protected:
    glm::vec3 color;
    float intensity;

public:
    Light(glm::vec3 color, float intensity);
    void setColor(glm::vec3);
    [[nodiscard]] glm::vec3 getColor() const;
    void setIntensity(float);
    [[nodiscard]] float getIntensity() const;

};

class DirectionalLight : public Light {
protected:
    glm::vec3 direction;

public:
    DirectionalLight(glm::vec3 color, float intensity, glm::vec3 direction);
    void setDirection(glm::vec3);
    [[nodiscard]] glm::vec3 getDirection() const;
};

class PointLight : public Light {
public:
    PointLight(glm::vec3 color, float intensity);
};

class SpotLight : public Light {
private:
    glm::vec3 direction;
    float cutoff;
    float outerCutoff;

public:
    SpotLight(glm::vec3 color, float intensity, glm::vec3 direction, float cutoff, float outerCutoff);
};
