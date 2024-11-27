#pragma once

#include <memory>
#include "../core/component.h"
#include "mesh.h"
#include "renderer.h"
#include "shader.h"
#include "material.h"
#include "armature.h"

class MeshRenderer : public Component, public Renderable {
private:
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;
    bool castShadows = true;
    Transform *transform;
    Armature *armature;
    RenderPass renderPass;

    void setUniforms(Shader *shader, glm::mat4 projectionMatrix, glm::mat4 viewMatrix);

public:
    explicit MeshRenderer(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);

    void init() override;
    void render() override;
    void renderShadow(glm::mat4 projectionMatrix, glm::mat4 viewMatrix) override;
    void remove() override;

    std::shared_ptr<Mesh> getMesh();
    void setMesh(std::shared_ptr<Mesh> mesh);

    Sphere getBoundingSphere();
};
