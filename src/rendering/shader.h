#pragma once


#include <string>
#include <filesystem>
#include "glm/glm.hpp"

enum class ShaderType {
    DEFERRED,
    FORWARD,
};

class Shader {
private:
    ShaderType type = ShaderType::DEFERRED;
    std::filesystem::path vertexPath;
    std::filesystem::path fragmentPath;
    unsigned int program = 0;

public:
    Shader(const std::string &vertexPath, const std::string &fragmentPath);
    ~Shader();

    ShaderType getType() const;
    void setType(ShaderType type);

    void upload();
    void bind() const;
    void unbind() const;

    void setUniform(const std::string &name, float value) const;
    void setUniform(const std::string &name, glm::vec2 value) const;
    void setUniform(const std::string &name, glm::vec3 value) const;
    void setUniform(const std::string &name, glm::mat3 value) const;
    void setUniform(const std::string &name, glm::mat4 value) const;

    static Shader *PBR;
    static Shader *DEFERRED_POINT_LIGHT;
    static Shader *DEFERRED_DIRECTIONAL_LIGHT;
    static Shader *HDR;
};