#pragma once


#include "../core/component.h"
#include "mesh.h"
#include "renderer.h"

class MeshRenderer : public Component, public Renderable {
private:
    Mesh *mesh;

public:
    explicit MeshRenderer(Mesh *mesh);

    void init() override;
    void render() override;
};
