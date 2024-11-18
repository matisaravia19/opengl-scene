#pragma once

#include "glm/glm.hpp"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    glm::vec2 texCoords;

    Vertex() = default;
};

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    unsigned int vao = 0;
    unsigned int vbo = 0;
    unsigned int ebo = 0;

    Mesh() = default;
    Mesh(unsigned int numVertices, unsigned int numIndices);
    ~Mesh();

    glm::vec3 getBoundingBox();
    void upload();
    bool isUploaded() const;
};
