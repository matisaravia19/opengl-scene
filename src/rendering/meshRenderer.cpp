#include "meshRenderer.h"

MeshRenderer::MeshRenderer(Mesh *mesh) {
    this->mesh = mesh;
}

void MeshRenderer::render() {

}

void MeshRenderer::init() {
    this->renderer->registerRenderable(this);
    mesh->upload();
}

MeshRenderer::~MeshRenderer() {
    this->renderer->removeRenderable(this);
}
