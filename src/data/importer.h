#pragma once

#include <string>
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "../core/entity.h"
#include "../rendering/mesh.h"

class Importer {
private:
    std::string path;
    Assimp::Importer importer;
    const aiScene *scene;
    std::vector<Entity*> entities;

    void loadMeshes(aiNode *node, Entity *entity);
    void loadEntities(aiNode *node, Transform *parent);

public:
    explicit Importer(std::string path);
    void import();
};
