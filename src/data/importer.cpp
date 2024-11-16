#include "importer.h"

#include "../core/input.h"
#include "../core/controllable.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "../rendering/meshRenderer.h"
#include "../rendering/camera.h"
#include "../rendering/light.h"
#include "../core/physicsComponent.h"

static inline glm::vec2 toVec2(aiVector3D aiVector) {
    return glm::vec2(aiVector.x, aiVector.y);
}

static inline glm::vec3 toVec3(aiVector3D aiVector) {
    return glm::vec3(aiVector.x, aiVector.y, aiVector.z);
}

static inline glm::vec3 toVec3(aiColor3D aiVector) {
    return glm::vec3(aiVector.r, aiVector.g, aiVector.b);
}

static inline glm::quat toQuat(aiQuaternion aiQuat) {
    return glm::quat(aiQuat.w, aiQuat.x, aiQuat.y, aiQuat.z);
}

static inline Transform *toTransform(aiMatrix4x4 aiMatrix) {
    aiVector3D scaling, position;
    aiQuaternion rotation;
    aiMatrix.Decompose(scaling, rotation, position);
    return new Transform(toVec3(position), toQuat(rotation), toVec3(scaling));
}

Importer::Importer(std::string path, Input *input) : input(input) {
    this->path = std::move(path);
}

static Vertex readVertex(aiMesh *mesh, int index) {
    Vertex vertex{};
    vertex.position = toVec3(mesh->mVertices[index]);
    vertex.normal = toVec3(mesh->mNormals[index]);
    if (mesh->mTextureCoords[0]) {
        vertex.texCoords = toVec2(mesh->mTextureCoords[0][index]);
    }
    return vertex;
}

static Mesh *readMesh(aiMesh *mesh) {
    auto result = new Mesh(mesh->mNumVertices, mesh->mNumFaces * 3);

    for (int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex = readVertex(mesh, i);
        result->vertices.push_back(vertex);
    }

    for (int i = 0; i < mesh->mNumFaces; i++) {
        auto face = mesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; j++) {
            result->indices.push_back(face.mIndices[j]);
        }
    }

    return result;
}

static Light *readLight(aiLight *light) {
    auto color = toVec3(light->mColorDiffuse);
    auto intensity = glm::length(color);
    color /= intensity;

    switch (light->mType) {
        case aiLightSourceType::aiLightSource_DIRECTIONAL:
            return new DirectionalLight(
                    color,
                    intensity,
                    toVec3(light->mDirection)
            );
        case aiLightSourceType::aiLightSource_SPOT:
            return new SpotLight(
                    color,
                    intensity,
                    toVec3(light->mDirection),
                    light->mAngleInnerCone,
                    light->mAngleOuterCone
            );
        default:
            return new PointLight(color, intensity);
    }
}

Entity *Importer::getEntity(const std::string &name) {
    for (auto &entity: entities) {
        if (entity->getName() == name) {
            return entity;
        }
    }

    return nullptr;
}

void Importer::loadNodes(aiNode *node, Transform *parent) {
    auto entity = new Entity(node->mName.C_Str());
    entities.push_back(entity);

    auto transform = toTransform(node->mTransformation);
    entity->addComponent(transform);

    if (parent) {
        parent->addChild(transform);
    }

    loadMeshes(node, entity);
    loadPhysics(node, entity);

    for (int i = 0; i < node->mNumChildren; i++) {
        loadNodes(node->mChildren[i], transform);
    }
}

void Importer::loadMeshes(aiNode *node, Entity *entity) {
    for (int i = 0; i < node->mNumMeshes; i++) {
        auto mesh = readMesh(scene->mMeshes[node->mMeshes[i]]);
        auto meshRenderer = new MeshRenderer(mesh);

        entity->addComponent(meshRenderer);
    }
}

void Importer::loadPhysics(aiNode *node, Entity *entity) {
    if (node->mNumMeshes == 0) return; // TODO: habria que traer info sobre que usa fisicas y que no desde el .glb

    static int id = 0;
    auto *physicsComponent = new PhysicsComponent(10, true);
    entity->addComponent(physicsComponent);

    id += 1;
}

void Importer::loadCameras() {
    Entity *entity = nullptr;
    for (int i = 0; i < scene->mNumCameras; i++) {
        auto camera = scene->mCameras[i];

        entity = getEntity(camera->mName.C_Str());
        if (!entity) {
            entity = new Entity(camera->mName.C_Str());
            entity->addComponent(new Transform(
                    toVec3(camera->mPosition),
                    toVec3(camera->mLookAt),
                    toVec3(camera->mUp)
            ));
            entities.push_back(entity);
        }

        entity->addComponent(new Camera(
                camera->mHorizontalFOV,
                camera->mClipPlaneNear,
                camera->mClipPlaneFar
        ));
    }
    if (!entity) return;
    entity->addComponent(new Controllable(input));
}

void Importer::loadLights() {
    for (int i = 0; i < scene->mNumLights; i++) {
        auto light = scene->mLights[i];

        auto entity = getEntity(light->mName.C_Str());
        if (!entity) {
            entity = new Entity(light->mName.C_Str());
            entity->addComponent(new Transform(
                    toVec3(light->mPosition),
                    toVec3(light->mDirection),
                    toVec3(light->mUp)
            ));
            entities.push_back(entity);
        }

        entity->addComponent(readLight(light));
    }
}

void Importer::load() {
    scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate);
    loadNodes(scene->mRootNode);
    loadCameras();
    loadLights();
}

std::vector<Entity *> Importer::getEntities() {
    return entities;
}