#pragma once

#include "../core/component.h"
#include "renderer.h"

class Skybox : public Component, public Renderable {
    unsigned int VAO, VBO;
    unsigned int dayTexture, nightTexture, sunsetTexture;
    unsigned int shaderProgram;
    double currentTime;

    struct TimeOfDaySettings {
        glm::vec3 sunPosition;
        float starBrightness;
        glm::vec3 ambientLight;
        float fogDensity;
    };

    void setupGeometry();
    void initializeShaders();

    void setMat4(const std::string &name, const glm::mat4 &mat) const {
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()),
                          1, GL_FALSE, &mat[0][0]);
    }
    void setVec3(const std::string &name, const glm::vec3 &value) const {
        glUniform3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]);
    }
    void setFloat(const std::string &name, const float value) const {
        glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), value);
    }
public:
    Skybox();
    void init() override;
    void update() override;
    TimeOfDaySettings calculateTimeSettings(float timeOfDay);
    void render() override;
};