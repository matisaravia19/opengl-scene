#pragma once


#include <string>
#include <filesystem>
#include "glm/glm.hpp"

class Texture {
protected:
    unsigned int id = 0;

public:
    virtual ~Texture();

    virtual void upload() = 0;
    virtual void bind(unsigned int slot);
};

class ColorTexture : public Texture {
private:
    glm::vec4 color;

public:
    explicit ColorTexture(glm::vec4 color);
    explicit ColorTexture(glm::vec3 color);
    explicit ColorTexture(float intensity);

    void upload() override;
};

class ImageTexture : public Texture {
private:
    std::filesystem::path path;

public:
    explicit ImageTexture(const std::string &path);

    void upload() override;
};