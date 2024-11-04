#include "material.h"

#include "renderer.h"

Material *Material::DEFAULT = new PbrMaterial("default");

Material::Material(std::string name) {
    this->name = name;
}

Shader *Material::getShader() {
    return shader;
}

PbrMaterial::PbrMaterial(std::string name) : Material(name) {
    shader = Shader::PBR;
}

void PbrMaterial::upload() {
    if (uploaded) {
        return;
    }

    shader->upload();

    if (albedo) {
        albedo->upload();
    }

    uploaded = true;
}

void PbrMaterial::bind() {
    shader->bind();

    if (albedo) {
        albedo->bind(0);
    }
}

void PbrMaterial::setAlbedo(std::shared_ptr<Texture> albedo) {
    this->albedo = std::move(albedo);
}

void PbrMaterial::setNormal(std::shared_ptr<Texture> normal) {
    this->normal = std::move(normal);
}

void PbrMaterial::setMetallic(std::shared_ptr<Texture> metallic) {
    this->metallic = std::move(metallic);
}

void PbrMaterial::setRoughness(std::shared_ptr<Texture> roughness) {
    this->roughness = std::move(roughness);
}

void PbrMaterial::setAO(std::shared_ptr<Texture> ao) {
    this->ao = std::move(ao);
}
