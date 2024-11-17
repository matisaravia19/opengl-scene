#include "importer.h"

#include <filesystem>
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "../rendering/meshRenderer.h"
#include "../rendering/light.h"
#include "../core/util.h"
#include "../core/controllable.h"

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

Importer::Importer(std::string path) {
    this->path = std::move(path);
    this->scene = nullptr;
}

Entity *Importer::getEntity(const std::string &name) {
    for (auto &entity: entities) {
        if (entity->getName() == name) {
            return entity;
        }
    }

    return nullptr;
}

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

void Importer::loadMaterials() {
    materials.reserve(scene->mNumMaterials);
    for (int i = 0; i < scene->mNumMaterials; i++) {
        auto material = readMaterial(scene->mMaterials[i]);
        materials.emplace_back(material);
    }
}

void Importer::loadMeshes() {
    meshes.reserve(scene->mNumMeshes);
    for (int i = 0; i < scene->mNumMeshes; i++) {
        auto mesh = readMesh(scene->mMeshes[i]);
        meshes.emplace_back(mesh);
    }
}

void Importer::addMeshRenderers(Entity *entity, aiNode *node) {
    for (int i = 0; i < node->mNumMeshes; i++) {
        auto mesh = meshes[node->mMeshes[i]];
        auto material = materials[scene->mMeshes[node->mMeshes[i]]->mMaterialIndex];
        auto meshRenderer = new MeshRenderer(mesh, material);

        entity->addComponent(meshRenderer);
    }
}

void Importer::loadNodes(aiNode *node, Transform *parent) {
    auto entity = new Entity(node->mName.C_Str());
    entities.push_back(entity);

    auto transform = toTransform(node->mTransformation);
    entity->addComponent(transform);

    if (parent) {
        parent->addChild(transform);
    }

    addMeshRenderers(entity, node);

    for (int i = 0; i < node->mNumChildren; i++) {
        loadNodes(node->mChildren[i], transform);
    }
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

void Importer::loadLights() {
    for (int i = 0; i < scene->mNumLights; i++) {
        auto light = scene->mLights[i];

        auto entity = getEntity(light);
        entity->addComponent(readLight(light));
    }
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
