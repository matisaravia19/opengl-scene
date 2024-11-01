#include "shader.h"

#include <fstream>
#include "glad/gl.h"

static std::string readFile(const std::string &path) {
    std::ifstream file(path);
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

static unsigned int loadShader(const std::string &path, unsigned int type) {
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
        throw std::runtime_error("Failed to compile shader: " + std::string(infoLog));
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
        throw std::runtime_error("Failed to link program: " + std::string(infoLog));
    }

    return program;
}

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath) {
    auto vertexShader = loadShader(vertexPath, GL_VERTEX_SHADER);
    auto fragmentShader = loadShader(fragmentPath, GL_FRAGMENT_SHADER);
    program = createProgram(vertexShader, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}
