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
#include "../debug/gizmoRenderer.h"
#include "../rendering/armature.h"
#include "../rendering/animationController.h"

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

static inline glm::mat4 toMat4(aiMatrix4x4 aiMatrix) {
    glm::mat4 result;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result[j][i] = aiMatrix[i][j];
        }
    }
    return result;
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

    for (int i = 0; i < mesh->mNumBones; i++) {
        for (int j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
            auto weight = mesh->mBones[i]->mWeights[j];
            auto vertexId = weight.mVertexId;
            auto vertex = &result->vertices[vertexId];
            vertex->addBoneData(i, weight.mWeight);
        }
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

    if (node->mMetaData->HasKey("lodDistance")) {
        double distance;
        node->mMetaData->Get("lodDistance", distance);

        lod->setDistance(distance);
    }

    entity->addComponent(lod);
}

void Importer::loadMeshComponents() {
    for (auto node: nodes) {
        if (node->mNumMeshes == 0) {
            continue;
        }

        auto entity = getEntity(node);
        addMeshRenderer(entity, node);

        if (!node->mMetaData) {
            continue;
        }

        if (node->mMetaData->HasKey("lod0")) {
            addLOD(entity, node);
        }
    }
}

#pragma endregion

#pragma region armatures

void Importer::loadArmatures() {
    for (int i = 0; i < scene->mNumMeshes; i++) {
        auto mesh = scene->mMeshes[i];
        if (mesh->mNumBones == 0) {
            continue;
        }

        auto node = mesh->mBones[0]->mArmature;
        auto entity = getEntity(node);

        // Fix scale being applied twice
        //entity->getTransform()->setScale(glm::vec3(1.0f));

        auto armature = new Armature();
        for (int j = 0; j < mesh->mNumBones; ++j) {
            auto bone = mesh->mBones[j];
            auto boneNode = bone->mNode;
            auto boneEntity = getEntity(boneNode);

            auto boneComponent = new Bone(toMat4(bone->mOffsetMatrix));
            boneEntity->addComponent(boneComponent);

            armature->addBone(boneComponent);
        }

        entity->addComponent(armature);
    }
}

#pragma endregion

#pragma region animations

static AnimationChannel readChannel(aiNodeAnim *channel) {
    AnimationChannel result;

    for (int i = 0; i < channel->mNumPositionKeys; i++) {
        auto key = channel->mPositionKeys[i];
        result.positions.emplace_back(key.mTime, toVec3(key.mValue));
    }

    for (int i = 0; i < channel->mNumRotationKeys; i++) {
        auto key = channel->mRotationKeys[i];
        result.rotations.emplace_back(key.mTime, toQuat(key.mValue));
    }

    for (int i = 0; i < channel->mNumScalingKeys; i++) {
        auto key = channel->mScalingKeys[i];
        result.scales.emplace_back(key.mTime, toVec3(key.mValue));
    }

    return result;
}

static AnimationController *getAnimationController(Entity *entity) {
    auto controller = entity->getComponent<AnimationController>();
    if (!controller) {
        controller = new AnimationController();
        entity->addComponent(controller);
    }

    return controller;
}

void Importer::loadAnimations() {
    for (int i = 0; i < scene->mNumAnimations; i++) {
        Animation animation;
        animation.name = scene->mAnimations[i]->mName.C_Str();
        animation.duration = scene->mAnimations[i]->mDuration;
        animation.ticksPerSecond = scene->mAnimations[i]->mTicksPerSecond;

        animation.channels.reserve(scene->mAnimations[i]->mNumChannels);
        for (int j = 0; j < scene->mAnimations[i]->mNumChannels; j++) {
            auto channel = readChannel(scene->mAnimations[i]->mChannels[j]);
            channel.bone = getEntity(scene->mAnimations[i]->mChannels[j]->mNodeName.C_Str())->getComponent<Bone>();
            animation.channels.push_back(channel);
        }

        auto entity = animation.channels[0].bone->getArmature()->getEntity();
        auto controller = getAnimationController(entity);
        controller->addAnimation(animation);
    }
}

#pragma endregion

#pragma region materials

static std::shared_ptr<Texture> readTexture(aiMaterial *material, aiTextureType type, unsigned int index) {
    aiString path;
    if (material->GetTexture(type, index, &path) == aiReturn_SUCCESS) {
        return std::make_shared<ImageTexture>(ImageTexture(path.C_Str()));
    } else {
        return nullptr;
    }
}

static glm::vec3 readMaterialColor(aiMaterial *material, const char *key, unsigned int type, unsigned int index) {
    aiColor3D color;
    if (material->Get(key, type, index, color) == aiReturn_SUCCESS) {
        return toVec3(color);
    } else {
        return glm::vec3(1.0f);
    }
}

static float readMaterialFloat(aiMaterial *material, const char *key, unsigned int type, unsigned int index) {
    float value;
    if (material->Get(key, type, index, value) == aiReturn_SUCCESS) {
        return value;
    } else {
        return 1.0f;
    }
}

static PbrMaterial *readPbrMaterial(aiMaterial *materialData) {
    aiString name;
    materialData->Get(AI_MATKEY_NAME, name);

    auto material = new PbrMaterial(name.C_Str());

    std::shared_ptr<Texture> albedo = readTexture(materialData, aiTextureType_DIFFUSE, 0);
    if (!albedo) {
        albedo = std::make_shared<ColorTexture>(readMaterialColor(materialData, AI_MATKEY_COLOR_DIFFUSE));
    }
    material->setAlbedo(albedo);

    std::shared_ptr<Texture> normal = readTexture(materialData, aiTextureType_NORMALS, 0);
    if (!normal) {
        normal = std::make_shared<ColorTexture>(glm::vec3(0.5f, 0.5f, 1.0f));
    }
    material->setNormal(normal);

    std::shared_ptr<Texture> metallicRoughness = readTexture(materialData, aiTextureType_METALNESS, 0);
    if (!metallicRoughness) {
        metallicRoughness = std::make_shared<ColorTexture>(glm::vec3(
                0.0f,
                readMaterialFloat(materialData, AI_MATKEY_METALLIC_FACTOR),
                readMaterialFloat(materialData, AI_MATKEY_ROUGHNESS_FACTOR)
        ));
    }
    material->setMetallicRoughness(metallicRoughness);

    std::shared_ptr<Texture> ao = readTexture(materialData, aiTextureType_AMBIENT_OCCLUSION, 0);
    if (!ao) {
        ao = std::make_shared<ColorTexture>(glm::vec3(1.0f));
    }
    material->setAO(ao);

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

#define LIGHT_INTENSITY_MULTIPLIER 0.001f

static Light *readLight(aiLight *light) {
    switch (light->mType) {
        case aiLightSourceType::aiLightSource_DIRECTIONAL:
            return new DirectionalLight(toVec3(light->mColorDiffuse) * LIGHT_INTENSITY_MULTIPLIER);
        case aiLightSourceType::aiLightSource_SPOT:
            return new SpotLight(
                    toVec3(light->mColorDiffuse) * LIGHT_INTENSITY_MULTIPLIER,
                    light->mAngleInnerCone,
                    light->mAngleOuterCone
            );
        default:
            return new PointLight(toVec3(light->mColorDiffuse) * LIGHT_INTENSITY_MULTIPLIER);
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
        entity->removeComponent<GizmoRenderer>();
    }
}

#pragma endregion

#pragma region nodes

void Importer::addPhysicsComponents(aiNode *node, Entity *entity) {
    if (node->mNumMeshes == 0) return; // TODO: habria que traer info sobre que usa fisicas y que no desde el .glb

    //auto *physicsComponent = new PhysicsComponent(10, true);
    //entity->addComponent(physicsComponent);
}

Entity *Importer::getEntity(aiNode *node) {
    if (nodeEntityMap.contains(node)) {
        return nodeEntityMap[node];
    } else {
        return nullptr;
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

    nodes.push_back(node);
    nodeEntityMap[node] = entity;

    auto transform = toTransform(node->mTransformation);
    entity->addComponent(transform);

    if (parent) {
        parent->addChild(transform);
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
    scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_FlipUVs | aiProcess_PopulateArmatureData);

    loadMaterials();
    loadMeshes();
    loadNodes(scene->mRootNode);
    loadMeshComponents();
    loadArmatures();
    loadAnimations();
    loadCameras();
    loadLights();
}

std::vector<Entity *> Importer::getEntities() {
    return entities;
}