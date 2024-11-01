#pragma once


#include <string>

class Shader {
private:
    unsigned int program;

public:
    Shader(const std::string &vertexPath, const std::string &fragmentPath);
    ~Shader();
};
