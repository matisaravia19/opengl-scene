#include "importer.h"

#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "../rendering/meshRenderer.h"

Importer::Importer(std::string path) {
    this->path = std::move(path);
}

Vertex Importer::readVertex(aiMesh *mesh, int index) {
    Vertex vertex{};
    vertex.position = glm::vec3(mesh->mVertices[index].x, mesh->mVertices[index].y, mesh->mVertices[index].z);
    vertex.normal = glm::vec3(mesh->mNormals[index].x, mesh->mNormals[index].y, mesh->mNormals[index].z);
    if (mesh->mTextureCoords[0]) {
        vertex.texCoords = glm::vec2(mesh->mTextureCoords[0][index].x, mesh->mTextureCoords[0][index].y);
    }
    return vertex;
}

Mesh Importer::readMesh(aiMesh *mesh) {
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

void Importer::loadMeshEntities(aiNode *node) {
    for (int i = 0; i < node->mNumMeshes; i++) {
        auto mesh = readMesh(scene->mMeshes[node->mMeshes[i]]);
        auto meshRenderer = new MeshRenderer(mesh);

        auto entity = Entity();
        entity.addComponent(meshRenderer);

        entities.push_back(entity);
    }
}

void Importer::loadEntities(aiNode *node) {
    loadMeshEntities(node);

    for (int i = 0; i < node->mNumChildren; i++) {
        loadEntities(node->mChildren[i]);
    }
}

void Importer::import() {
    scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate);
    loadEntities(scene->mRootNode);
}