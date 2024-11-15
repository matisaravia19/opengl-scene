#pragma once

#include <string>
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "../core/entity.h"
#include "../rendering/mesh.h"

class Input;

class Importer {
private:
    std::string path;
    Assimp::Importer importer;
    const aiScene *scene;
    std::vector<Entity *> entities;

    // For the camera's Controllable component
    Input *input;

    Entity *getEntity(const std::string &name);

    void loadNodes(aiNode *node, Transform *parent = nullptr);
    void loadMeshes(aiNode *node, Entity *entity);
    void loadPhysics(aiNode *node, Entity *entity);
    void loadCameras();
    void loadLights();

public:
    explicit Importer(std::string path, Input *input);

    void load();
    std::vector<Entity *> getEntities();
};
