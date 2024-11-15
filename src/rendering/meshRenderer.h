#pragma once


#include "../core/component.h"
#include "mesh.h"
#include "renderer.h"
#include "shader.h"

class MeshRenderer : public Component, public Renderable {
private:
    Mesh *mesh;
    Shader *shader;
    Transform *transform;

public:
    explicit MeshRenderer(Mesh *mesh);

    void init() override;
    void render() override;
    void remove() override;

    Mesh *getMesh() { return mesh; }
};
