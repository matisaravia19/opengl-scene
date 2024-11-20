#pragma once

#include <vector>
#include <memory>
#include "../core/component.h"
#include "mesh.h"
#include "meshRenderer.h"

#define MAX_LODS 5

class LOD : public Component {
private:
    std::vector<std::shared_ptr<Mesh>> meshes;
    float distanceBetweenLODs = 10;

    MeshRenderer *renderer;

public:
    void addMesh(std::shared_ptr<Mesh> mesh);

    void init() override;
    void update() override;
};
