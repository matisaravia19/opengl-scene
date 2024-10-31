#include "importer.h"

#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "../rendering/meshRenderer.h"

static inline glm::vec2 toVec2(aiVector3D aiVector) {
    return glm::vec2(aiVector.x, aiVector.y);
}

static inline glm::vec3 toVec3(aiVector3D aiVector) {
    return glm::vec3(aiVector.x, aiVector.y, aiVector.z);
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

Importer::Importer(std::string path) {
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

static Mesh readMesh(aiMesh *mesh) {
    auto result = Mesh(mesh->mNumVertices, mesh->mNumFaces * 3);

    for (int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex = readVertex(mesh, i);
        result.vertices.push_back(vertex);
    }

    for (int i = 0; i < mesh->mNumFaces; i++) {
        auto face = mesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; j++) {
            result.indices.push_back(face.mIndices[j]);
        }
    }

    return result;
}

void Importer::loadMeshes(aiNode *node, Entity *entity) {
    for (int i = 0; i < node->mNumMeshes; i++) {
        auto mesh = readMesh(scene->mMeshes[node->mMeshes[i]]);
        auto meshRenderer = new MeshRenderer(mesh);

        entity->addComponent(meshRenderer);
    }
}

void Importer::loadEntities(aiNode *node, Transform *parent) {
    auto transform = toTransform(node->mTransformation);
    if (parent) {
        parent->addChild(transform);
    }

    auto entity = new Entity();
    entity->addComponent(transform);
    entities.push_back(entity);

    loadMeshes(node, entity);

    for (int i = 0; i < node->mNumChildren; i++) {
        loadEntities(node->mChildren[i], transform);
    }
}

void Importer::import() {
    scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate);
    loadEntities(scene->mRootNode, nullptr);
}