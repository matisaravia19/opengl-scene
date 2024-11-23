#pragma once

#include <string>
#include "glm/glm.hpp"

#define MAX_BONES 64
#define MAX_BONES_PER_VERTEX 4

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    glm::vec2 texCoords;
    int boneIds[MAX_BONES_PER_VERTEX];
    float weights[MAX_BONES_PER_VERTEX];

    Vertex();

    void addBoneData(int boneId, float weight);
};

struct Bone {
    std::string name;
    glm::mat4 offsetMatrix;
};

struct Mesh {
    std::string name;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Bone> bones;

    unsigned int vao = 0;
    unsigned int vbo = 0;
    unsigned int ebo = 0;

    Mesh() = default;
    Mesh(std::string name, unsigned int numVertices, unsigned int numIndices);
    ~Mesh();

    glm::vec3 getBoundingBox();
    void upload();
    bool isUploaded() const;
};
