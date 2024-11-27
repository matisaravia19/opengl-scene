#pragma once


#include <unordered_set>
#include "glm/glm.hpp"
#include "../core/component.h"
#include "shader.h"
#include "renderer.h"

#define SHADOW_MAP_SIZE 2048
#define SHADOW_FAR_PLANE 200.0f
#define SHADOW_NEAR_PLANE 0.1f

#define DIRECTIONAL_LIGHT_SHADOW_DISTANCE 40.0f

class Light : public Component {
protected:
    glm::vec3 color;
    Shader *deferredShader;

    glm::vec<2, int> shadowMapSize = {SHADOW_MAP_SIZE, SHADOW_MAP_SIZE};
    unsigned int shadowMap;
    unsigned int shadowMapFBO;

    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    Frustum shadowFrustum;

    void createShadowMap();
    virtual void updateProjectionMatrix() = 0;
    virtual void updateViewMatrix() = 0;
    void updateShadowFrustum();

    virtual void setUniforms();

public:

    explicit Light(glm::vec3 color);
    void init() override;
    void update() override;
    void remove() override;
    virtual void renderDeferred();
    void renderShadow(const std::unordered_set<Renderable *> &renderables);

    glm::mat4 getProjectionMatrix() const;
    glm::mat4 getViewMatrix() const;
    Frustum getShadowFrustum() const;

    void setColor(glm::vec3);
    [[nodiscard]] glm::vec3 getColor() const;
};

class DirectionalLight : public Light {
protected:
    void updateProjectionMatrix() override;
    void updateViewMatrix() override;

    void setUniforms() override;

public:
    explicit DirectionalLight(glm::vec3 color);
    void setDirection(glm::vec3);

    void init() override;
};

class PointLight : public Light {
protected:
    void updateProjectionMatrix() override;
    void updateViewMatrix() override;

    void setUniforms() override;

public:
    PointLight(glm::vec3 color);

    void init() override;
};

class SpotLight : public Light {
private:
    float innerAngle;
    float innerCutoff;
    float outerAngle;
    float outerCutoff;

protected:
    void updateProjectionMatrix() override;
    void updateViewMatrix() override;

    void setUniforms() override;

public:
    SpotLight(glm::vec3 color, float innerAngle, float outerAngle);

    void init() override;
};
