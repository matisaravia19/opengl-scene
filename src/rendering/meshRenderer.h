#pragma once


#include "../core/component.h"
#include "mesh.h"
#include "renderer.h"

class MeshRenderer : public Component, public Renderable {
private:
    Mesh *mesh;

public:
    explicit MeshRenderer(Mesh *mesh);
    ~MeshRenderer() override;

    void init() override;
    void render() override;
};
