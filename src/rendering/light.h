#pragma once


#include "glm/glm.hpp"
#include "../core/component.h"
#include "shader.h"

class Light : public Component {
protected:
    glm::vec3 color;
    float intensity;
    Shader *deferredShader;

public:
    Light(glm::vec3 color, float intensity);

    void init() override;
    void remove() override;

    virtual void renderDeferred() = 0;
};

class DirectionalLight : public Light {
private:
    glm::vec3 direction;

public:
    DirectionalLight(glm::vec3 color, float intensity, glm::vec3 direction);

    void init() override;

    void renderDeferred() override;
};

class PointLight : public Light {
public:
    PointLight(glm::vec3 color, float intensity);

    void init() override;

    void renderDeferred() override;
};

class SpotLight : public Light {
private:
    glm::vec3 direction;
    float cutoff;
    float outerCutoff;

public:
    SpotLight(glm::vec3 color, float intensity, glm::vec3 direction, float cutoff, float outerCutoff);

    void renderDeferred() override;
};
