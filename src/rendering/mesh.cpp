#include "mesh.h"

Mesh::Mesh(unsigned int numVertices, unsigned int numIndices) {
    vertices.reserve(numVertices);
    indices.reserve(numIndices);
}
