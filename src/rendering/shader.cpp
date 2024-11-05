#include "shader.h"

#include <fstream>
#include "glad/gl.h"
#include "shadinclude.hpp"
#include "../core/util.h"
#include "../core/constants.h"

static std::string readFile(const std::filesystem::path &path) {
    return Shadinclude::load(path.string());
}

static unsigned int loadShader(const std::filesystem::path &path, unsigned int type) {
    auto source = readFile(path);
    auto shader = glCreateShader(type);

    const char *cSource = source.c_str();
    glShaderSource(shader, 1, &cSource, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        LOG_ERROR("Failed to compile shader: %s", infoLog);
    }

    return shader;
}

static unsigned int createProgram(unsigned int vertexShader, unsigned int fragmentShader) {
    auto program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        LOG_ERROR("Failed to link program: %s", infoLog);
    }

    return program;
}

Shader *Shader::PBR = new Shader("shaders/standard.vert", "shaders/pbr.frag");

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath) {
    this->vertexPath = RESOURCES_PATH / vertexPath;
    this->fragmentPath = RESOURCES_PATH / fragmentPath;
}

Shader::~Shader() {
    glDeleteProgram(program);
}

void Shader::bind() const {
    glUseProgram(program);
}

void Shader::unbind() const {
    glUseProgram(0);
}

void Shader::setUniform(const std::string &name, glm::vec3 value) const {
    auto location = glGetUniformLocation(program, name.c_str());
    glUniform3fv(location, 1, &value[0]);
}

void Shader::setUniform(const std::string &name, glm::mat3 value) const {
    auto location = glGetUniformLocation(program, name.c_str());
    glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
}

void Shader::setUniform(const std::string &name, glm::mat4 value) const {
    auto location = glGetUniformLocation(program, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
}

void Shader::upload() {
    if (program) {
        return;
    }

    auto vertexShader = loadShader(vertexPath, GL_VERTEX_SHADER);
    auto fragmentShader = loadShader(fragmentPath, GL_FRAGMENT_SHADER);
    program = createProgram(vertexShader, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}
