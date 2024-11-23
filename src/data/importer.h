#pragma once

#include <string>
#include <map>
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

    std::vector<aiNode *> nodes;
    std::map<aiNode *, Entity *> nodeEntityMap;

    Entity *getEntity(aiNode *node);
    Entity *getEntity(const std::string &name);
    Entity *getEntity(const aiCamera *camera);
    Entity *getEntity(const aiLight *light);

    std::shared_ptr<Mesh> getMesh(const std::string &name);

    void addMeshComponents(Entity *entity, aiNode *node);
    void addMeshRenderer(Entity *entity, aiNode *node);
    void addLOD(Entity *entity, aiNode *node);
    void addPhysicsComponents(aiNode *node, Entity *entity);

    void loadMaterials();
    void loadMeshes();
    void loadNodes(aiNode *node, Transform *parent = nullptr);
    void loadMeshComponents();
    void loadCameras();
    void loadLights();

public:
    explicit Importer(std::string path);

    void load();

    std::vector<Entity *> getEntities();
};
