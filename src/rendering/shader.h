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
    void setUniform(const std::string &name, glm::vec4 value) const;
    void setUniform(const std::string &name, glm::mat3 value) const;
    void setUniform(const std::string &name, glm::mat4 value) const;
    void setUniform(const std::string &name, glm::mat4 *values, int count) const;
    void setUniform(const std::string &name, int value) const;

    static Shader *PBR;
    static Shader *GIZMO;

    static Shader *DEFERRED_POINT_LIGHT;
    static Shader *DEFERRED_DIRECTIONAL_LIGHT;
    static Shader *DEFERRED_SPOT_LIGHT;
    static Shader *SHADOW;

    static Shader *HDR;
};