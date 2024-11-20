#include "importer.h"

#include <filesystem>
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "../rendering/meshRenderer.h"
#include "../rendering/light.h"
#include "../core/util.h"
#include "../core/controllable.h"
#include "../core/physicsComponent.h"
#include "../rendering/lod.h"

#pragma region conversions

static inline glm::vec2 toVec2(aiVector3D aiVector) {
    return glm::vec2(aiVector.x, aiVector.y);
}

static inline glm::vec3 toVec3(aiVector3D aiVector) {
    return glm::vec3(aiVector.x, aiVector.y, aiVector.z);
}

static inline glm::vec3 toVec3(const aiColor3D &aiColor) {
    return glm::vec3(aiColor.r, aiColor.g, aiColor.b);
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

#pragma endregion

#pragma region meshes

static Vertex readVertex(aiMesh *mesh, int index) {
    Vertex vertex{};
    vertex.position = toVec3(mesh->mVertices[index]);
    vertex.normal = toVec3(mesh->mNormals[index]);
    vertex.tangent = toVec3(mesh->mTangents[index]);
    vertex.bitangent = toVec3(mesh->mBitangents[index]);
    if (mesh->mTextureCoords[0]) {
        vertex.texCoords = toVec2(mesh->mTextureCoords[0][index]);
    }
    return vertex;
}

static Mesh *readMesh(aiMesh *mesh) {
    auto result = new Mesh(mesh->mName.C_Str(), mesh->mNumVertices, mesh->mNumFaces * 3);

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

void Importer::loadMeshes() {
    meshes.reserve(scene->mNumMeshes);
    for (int i = 0; i < scene->mNumMeshes; i++) {
        auto mesh = readMesh(scene->mMeshes[i]);
        meshes.emplace_back(mesh);
    }
}

std::shared_ptr<Mesh> Importer::getMesh(const std::string &name) {
    for (auto &mesh: meshes) {
        if (mesh->name == name) {
            return mesh;
        }
    }
    return nullptr;
}

void Importer::addMeshRenderer(Entity *entity, aiNode *node) {
    auto mesh = meshes[node->mMeshes[0]];
    auto material = materials[scene->mMeshes[node->mMeshes[0]]->mMaterialIndex];
    auto meshRenderer = new MeshRenderer(mesh, material);

    entity->addComponent(meshRenderer);
}

void Importer::addLOD(Entity *entity, aiNode *node) {
    auto lod = new LOD();

    for (int i = 0; i < MAX_LODS; i++) {
        auto propertyName = std::format("lod{}", i);
        if (!node->mMetaData->HasKey(propertyName.c_str())) {
            break;
        }

        aiMetadata lodData;
        node->mMetaData->Get(propertyName, lodData);

        aiString lodName;
        lodData.Get("name", lodName);
        
        auto mesh = getMesh(lodName.C_Str());
        lod->addMesh(mesh);
    }

    entity->addComponent(lod);
}

void Importer::addMeshComponents(Entity *entity, aiNode *node) {
    addMeshRenderer(entity, node);

    if (!node->mMetaData) {
        return;
    }

    if (node->mMetaData->HasKey("lod0")) {
        addLOD(entity, node);
    }
}

#pragma endregion

#pragma region materials

static std::shared_ptr<Texture> readTexture(aiMaterial *material, aiTextureType type, unsigned int index) {
    aiString path;
    if (material->GetTexture(type, index, &path) == aiReturn_SUCCESS) {
        return std::make_shared<ImageTexture>(ImageTexture(path.C_Str()));
    } else {
        return std::make_shared<ColorTexture>(ColorTexture(glm::vec3(1.0f)));
    }
}

static PbrMaterial *readPbrMaterial(aiMaterial *materialData) {
    aiString name;
    materialData->Get(AI_MATKEY_NAME, name);

    auto material = new PbrMaterial(name.C_Str());
    material->setAlbedo(readTexture(materialData, aiTextureType_DIFFUSE, 0));
    material->setNormal(readTexture(materialData, aiTextureType_NORMALS, 0));
    material->setMetallicRoughness(readTexture(materialData, aiTextureType_METALNESS, 0));
    material->setAO(readTexture(materialData, aiTextureType_AMBIENT_OCCLUSION, 0));

    return material;
}

static Material *readMaterial(aiMaterial *material) {
    aiShadingMode shadingModel;
    material->Get<aiShadingMode>(AI_MATKEY_SHADING_MODEL, shadingModel);

    switch (shadingModel) {
        case aiShadingMode_PBR_BRDF:
            return readPbrMaterial(material);
        default:
            LOG_ERROR("Unsupported shading model");
            return Material::DEFAULT;
    }
}

void Importer::loadMaterials() {
    materials.reserve(scene->mNumMaterials);
    for (int i = 0; i < scene->mNumMaterials; i++) {
        auto material = readMaterial(scene->mMaterials[i]);
        materials.emplace_back(material);
    }
}

#pragma endregion

#pragma region lights

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

Entity *Importer::getEntity(const aiLight *light) {
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

    return entity;
}

void Importer::loadLights() {
    for (int i = 0; i < scene->mNumLights; i++) {
        auto light = scene->mLights[i];

        auto entity = getEntity(light);
        entity->addComponent(readLight(light));
    }
}

#pragma endregion

#pragma region cameras

Entity *Importer::getEntity(const aiCamera *camera) {
    auto entity = getEntity(camera->mName.C_Str());
    if (!entity) {
        entity = new Entity(camera->mName.C_Str());
        entity->addComponent(new Transform(
                toVec3(camera->mPosition),
                toVec3(camera->mLookAt),
                toVec3(camera->mUp)
        ));
        entities.push_back(entity);
    }

    return entity;
}

void Importer::loadCameras() {
    for (int i = 0; i < scene->mNumCameras; i++) {
        auto camera = scene->mCameras[i];

        auto entity = getEntity(camera);
        entity->addComponent(new Camera(
                camera->mHorizontalFOV,
                camera->mClipPlaneNear,
                camera->mClipPlaneFar
        ));
        entity->addComponent(new Controllable());
    }
}

#pragma endregion

#pragma region nodes

void Importer::addPhysicsComponents(aiNode *node, Entity *entity) {
    if (node->mNumMeshes == 0) return; // TODO: habria que traer info sobre que usa fisicas y que no desde el .glb

    //auto *physicsComponent = new PhysicsComponent(10, true);
    //entity->addComponent(physicsComponent);
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

    if (node->mNumMeshes > 0) {
        addMeshComponents(entity, node);
        addPhysicsComponents(node, entity);
    }

    for (int i = 0; i < node->mNumChildren; i++) {
        loadNodes(node->mChildren[i], transform);
    }
}

#pragma endregion

Importer::Importer(std::string path) {
    this->path = std::move(path);
    this->scene = nullptr;
}

void Importer::load() {
    scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_CalcTangentSpace);

    loadMaterials();
    loadMeshes();
    loadNodes(scene->mRootNode);
    loadCameras();
    loadLights();
}

std::vector<Entity *> Importer::getEntities() {
    return entities;
}
