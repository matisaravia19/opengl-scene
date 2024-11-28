#pragma once


#include <memory>
#include "../core/component.h"
#include "material.h"
#include "meshRenderer.h"

class TreeRenderer : public MeshRenderer {
private:
    std::shared_ptr<Mesh> billboardMesh;
    std::shared_ptr<Material> billboardMaterial;

    bool isInBillboardMode = false;

public:
    TreeRenderer(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> baseMaterial,
                 std::shared_ptr<Mesh> billboardMesh, std::shared_ptr<Material> billboardMaterial);

    void init() override;

    void update() override;

    void render() override;

    void renderShadow(const Light *light) override;
};
