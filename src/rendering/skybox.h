#pragma once

#include "../core/component.h"
#include "renderer.h"

// TODO: Use existing shader class.
class Skybox : public Component, public Renderable {
    unsigned int VAO, VBO;
    unsigned int dayTexture, nightTexture, sunsetTexture;
    unsigned int shaderProgram;
    float currentTime;
    std::string vertexShaderSource, fragmentShaderSource;

    struct TimeOfDaySettings {
        glm::vec3 sunPosition;
        float starBrightness;
        glm::vec3 ambientLight;
        float fogDensity;
    };

    void setupGeometry();
    void initializeShaders();
    void readShaderSource(const std::string& vertPath, const std::string& fragPath);

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
    Skybox(const std::string& vertPath, const std::string& fragPath);
    void init() override;
    void update() override;
    TimeOfDaySettings calculateTimeSettings(float timeOfDay);
    void render() override;
};