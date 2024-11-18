#pragma once


#include <string>
#include <filesystem>
#include "glm/glm.hpp"

class Shader {
private:
    std::filesystem::path vertexPath;
    std::filesystem::path fragmentPath;
    unsigned int program = 0;

public:
    Shader(const std::string &vertexPath, const std::string &fragmentPath);
    ~Shader();

    void upload();
    void bind() const;
    void unbind() const;

    void setUniform(const std::string &name, glm::vec3 value) const;
    void setUniform(const std::string &name, glm::mat3 value) const;
    void setUniform(const std::string &name, glm::mat4 value) const;

    static Shader *PBR;
};