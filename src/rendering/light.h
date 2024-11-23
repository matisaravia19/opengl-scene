#pragma once


#include "glm/glm.hpp"
#include "../core/component.h"
#include "shader.h"

class Light : public Component {
protected:
    glm::vec3 color;
    Shader *deferredShader;

public:
    explicit Light(glm::vec3 color);

    void init() override;
    void remove() override;

    virtual void renderDeferred() = 0;

    void setColor(glm::vec3);
    [[nodiscard]] glm::vec3 getColor() const;
};

class DirectionalLight : public Light {
public:
    explicit DirectionalLight(glm::vec3 color);
    void setDirection(glm::vec3);

    void init() override;

    void renderDeferred() override;
};

class PointLight : public Light {
public:
    PointLight(glm::vec3 color);

    void init() override;

    void renderDeferred() override;
};

class SpotLight : public Light {
private:
    float innerCutoff;
    float outerCutoff;

public:
    SpotLight(glm::vec3 color, float innerAngle, float outerAngle);

    void init() override;

    void renderDeferred() override;
};
