#include "meshRenderer.h"

#include <utility>
#include "../core/entity.h"

MeshRenderer::MeshRenderer(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) {
    this->mesh = std::move(mesh);
    this->material = std::move(material);
}

void MeshRenderer::setUniforms() {
    auto shader = material->getShader();

    auto modelMatrix = transform->getModelMatrix();
    shader->setUniform("modelMatrix", modelMatrix);

    auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelMatrix)));
    shader->setUniform("normalMatrix", normalMatrix);

    auto camera = Renderer::getActive()->getCamera();
    shader->setUniform("viewMatrix", camera->getView());
    shader->setUniform("projectionMatrix", camera->getProjection());
    shader->setUniform("cameraPosition", camera->getEntity()->getTransform()->getPosition());
}

void MeshRenderer::render() {
    material->bind();

    setUniforms();

    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void MeshRenderer::init() {
    Component::init();

    transform = getEntity()->getTransform();

    Renderer::getActive()->registerRenderable(this);
    mesh->upload();
    material->upload();
}

void MeshRenderer::remove() {
    Component::remove();

    Renderer::getActive()->removeRenderable(this);
}

std::shared_ptr<Mesh> MeshRenderer::getMesh() {
    return mesh;
}

void MeshRenderer::setMesh(std::shared_ptr<Mesh> mesh) {
    this->mesh = mesh;
    if (getLifeState() == ComponentLifeState::INITIALIZED) {
        mesh->upload();
    }
}
