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
    std::vector<Entity> entities;

    static Vertex readVertex(aiMesh *mesh, int index);
    static Mesh readMesh(aiMesh *mesh);

    void loadMeshEntities(aiNode *node);
    void loadEntities(aiNode *node);

public:
    explicit Importer(std::string path);
    void import();
};
