#include "mesh.h"

#include "glad/gl.h"

Vertex::Vertex() {
    position = glm::vec3(0);
    normal = glm::vec3(0);
    tangent = glm::vec3(0);
    bitangent = glm::vec3(0);
    texCoords = glm::vec2(0);
    for (int i = 0; i < MAX_BONES_PER_VERTEX; i++) {
        boneIds[i] = -1;
        weights[i] = 0.0f;
    }
}

void Vertex::addBoneData(int boneId, float weight) {
    for (int i = 0; i < MAX_BONES_PER_VERTEX; i++) {
        if (boneIds[i] == -1) {
            boneIds[i] = boneId;
            weights[i] = weight;
            return;
        }
    }
}

Mesh::Mesh(std::string name, unsigned int numVertices, unsigned int numIndices) {
    this->name = std::move(name);
    vertices.reserve(numVertices);
    indices.reserve(numIndices);
}

void Mesh::upload() {
    if (isUploaded()) {
        return;
    }

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, tangent));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, bitangent));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, texCoords));

    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, MAX_BONES_PER_VERTEX, GL_INT, sizeof(Vertex), (void *) offsetof(Vertex, boneIds));

    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, MAX_BONES_PER_VERTEX, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, weights));

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

Mesh::~Mesh() {
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
}

glm::vec3 Mesh::getBoundingBox() {
    auto bb = glm::vec3(0);

    for (const auto &vertex: vertices) {
        bb.x = std::max(bb.x, std::abs(vertex.position.x));
        bb.y = std::max(bb.y, std::abs(vertex.position.y));
        bb.z = std::max(bb.z, std::abs(vertex.position.z));
    }

    return bb;
}

bool Mesh::isUploaded() const {
    return vao != 0;
}
