#include "meshRenderer.h"

MeshRenderer::MeshRenderer(Mesh *mesh) {
    this->mesh = mesh;
}

void MeshRenderer::render() {

}

void MeshRenderer::init() {
    mesh->upload();
}
