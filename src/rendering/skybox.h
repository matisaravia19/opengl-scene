#pragma once

#include "../core/component.h"
#include "renderer.h"
#include "shader.h"

class Skybox : public Component, public Renderable {
    unsigned int VAO, VBO;
    unsigned int dayTexture, nightTexture, sunsetTexture;
    float currentTime;

    Shader *skyboxShader;

    struct TimeOfDaySettings {
        glm::vec3 sunPosition;
        float starBrightness;
        glm::vec3 ambientLight;
        float fogDensity;
    };

    void setupGeometry();
public:
    Skybox(const std::string& vertPath, const std::string& fragPath);
    ~Skybox() override;

    void init() override;
    void update() override;
    static TimeOfDaySettings calculateTimeSettings(float timeOfDay);
    void render() override;
};