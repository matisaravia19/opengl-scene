#pragma once

#include <memory>
#include "../core/component.h"
#include "mesh.h"
#include "renderer.h"
#include "shader.h"
#include "material.h"

class MeshRenderer : public Component, public Renderable {
private:
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;
    Transform *transform;

    void setUniforms();

public:
    explicit MeshRenderer(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);

    void init() override;
    void render() override;
    void remove() override;
};
