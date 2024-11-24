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

static Mesh *createCube() {
    std::vector<glm::vec3> vertices = {
            glm::vec3(-0.5f, -0.5f, -0.5f),
            glm::vec3(0.5f, -0.5f, -0.5f),
            glm::vec3(0.5f, 0.5f, -0.5f),
            glm::vec3(-0.5f, 0.5f, -0.5f),
            glm::vec3(-0.5f, -0.5f, 0.5f),
            glm::vec3(0.5f, -0.5f, 0.5f),
            glm::vec3(0.5f, 0.5f, 0.5f),
            glm::vec3(-0.5f, 0.5f, 0.5f)
    };

    std::vector<unsigned int> indices = {
            0, 1, 2, 2, 3, 0,
            1, 5, 6, 6, 2, 1,
            7, 6, 5, 5, 4, 7,
            4, 0, 3, 3, 7, 4,
            3, 2, 6, 6, 7, 3,
            4, 5, 1, 1, 0, 4
    };

    auto mesh = new Mesh("Cube", vertices.size(), indices.size());
    for (auto position: vertices) {
        Vertex vertex;
        vertex.position = position;
        mesh->vertices.push_back(vertex);
    }

    mesh->indices = indices;

    return mesh;
}

static Mesh *createQuad() {
    std::vector<glm::vec3> vertices = {
            glm::vec3(-1.0f, -1.0f, 0.0f),
            glm::vec3(1.0f, -1.0f, 0.0f),
            glm::vec3(1.0f, 1.0f, 0.0f),
            glm::vec3(-1.0f, 1.0f, 0.0f)
    };

    std::vector<unsigned int> indices = {
            0, 1, 2, 2, 3, 0
    };

    auto mesh = new Mesh("Quad", vertices.size(), indices.size());
    for (auto position: vertices) {
        Vertex vertex;
        vertex.position = position;
        mesh->vertices.push_back(vertex);
    }

    mesh->indices = indices;

    return mesh;
}

Mesh *Mesh::QUAD = createQuad();
Mesh *Mesh::CUBE = createCube();

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
