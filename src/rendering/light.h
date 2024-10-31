#pragma once


#include "glm/glm.hpp"
#include "../core/component.h"

class Light : public Component {
protected:
    glm::vec3 color;
    float intensity;

public:
    Light(glm::vec3 color, float intensity);
};

class DirectionalLight : public Light {
private:
    glm::vec3 direction;

public:
    DirectionalLight(glm::vec3 color, float intensity, glm::vec3 direction);
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
