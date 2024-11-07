#pragma once

#include <string>
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "../core/entity.h"
#include "../rendering/mesh.h"
#include "../rendering/material.h"

class Input;

class Importer {
private:
    std::string path;
    Assimp::Importer importer;
    const aiScene *scene;

    std::vector<std::shared_ptr<Material>> materials;
    std::vector<std::shared_ptr<Mesh>> meshes;
    std::vector<Entity *> entities;

    Entity *getEntity(const std::string &name);
    Entity *getEntity(const aiCamera *camera);
    Entity *getEntity(const aiLight *light);

    void addMeshRenderers(Entity *entity, aiNode *node);

    void loadMaterials();
    void loadMeshes();
    void loadNodes(aiNode *node, Transform *parent = nullptr);
    void loadCameras();
    void loadLights();

public:
    explicit Importer(std::string path);

    void load();
    std::vector<Entity *> getEntities();
};
