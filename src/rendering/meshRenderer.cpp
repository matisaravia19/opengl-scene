#include "meshRenderer.h"

#include <utility>
#include "../core/entity.h"

MeshRenderer::MeshRenderer(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) {
    this->mesh = std::move(mesh);
    this->material = std::move(material);
}

void MeshRenderer::render() {
    material->bind();

    auto shader = material->getShader();
    auto camera = Renderer::getActive()->getCamera();
    shader->setUniform("model", transform->getModelMatrix());
    shader->setUniform("view", camera->getView());
    shader->setUniform("projection", camera->getProjection());

    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void MeshRenderer::init() {
    transform = getEntity()->getTransform();

    Renderer::getActive()->registerRenderable(this);
    mesh->upload();
    material->upload();
}

void MeshRenderer::remove() {
    Renderer::getActive()->removeRenderable(this);
}
