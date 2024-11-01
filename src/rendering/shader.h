#pragma once


#include <string>
#include "glm/glm.hpp"

class Shader {
private:
    unsigned int program;

public:
    Shader(const std::string &vertexPath, const std::string &fragmentPath);
    ~Shader();

    void use() const;
    void disable() const;

    void setUniform(const std::string &name, glm::mat4 value) const;
};
