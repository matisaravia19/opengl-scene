#pragma once

#include <memory>
#include "shader.h"
#include "texture.h"

class Material {
protected:
    std::string name;
    Shader *shader = nullptr;

    bool uploaded = false;

public:
    explicit Material(std::string name);

    virtual void upload() = 0;
    virtual void bind() = 0;

    Shader *getShader();

    static Material *DEFAULT;
};

class PbrMaterial : public Material {
private:
    std::shared_ptr<Texture> albedo;
    std::shared_ptr<Texture> normal;
    std::shared_ptr<Texture> metallicRoughness;
    std::shared_ptr<Texture> ao;

public:
    explicit PbrMaterial(std::string name);

    void upload() override;
    void bind() override;

    void setAlbedo(std::shared_ptr<Texture> albedo);
    void setNormal(std::shared_ptr<Texture> normal);
    void setMetallicRoughness(std::shared_ptr<Texture> metallicRoughness);
    void setAO(std::shared_ptr<Texture> ao);
};