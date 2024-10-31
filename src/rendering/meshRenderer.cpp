#include "meshRenderer.h"

#include <utility>

MeshRenderer::MeshRenderer(Mesh mesh) {
    this->mesh = std::move(mesh);
}

void MeshRenderer::render() {

}
