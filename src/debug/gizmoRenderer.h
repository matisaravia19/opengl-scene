#pragma once


#include "../core/component.h"
#include "../rendering/renderer.h"
#include "../rendering/mesh.h"

class GizmoRenderer : public Component, Renderable {
private:
    Shader *shader;
    glm::vec4 color;
    float scale = 0.25f;

    static std::unique_ptr<Mesh> GIZMO_MESH;

public:
    explicit GizmoRenderer(glm::vec4 color);

    void init() override;
    void render() override;
    void remove() override;
};
