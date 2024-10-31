#include "meshRenderer.h"

#include <utility>

MeshRenderer::MeshRenderer(Mesh mesh) {
    this->mesh = std::move(mesh);
}

void MeshRenderer::render() {

}

void MeshRenderer::init() {
    this->renderer->registerRenderable(this);
}

MeshRenderer::~MeshRenderer() {
    this->renderer->removeRenderable(this);
}
