#include "gizmoRenderer.h"

#include "../core/entity.h"
#include "../rendering/meshRenderer.h"

GizmoRenderer::GizmoRenderer(glm::vec4 color) {
    this->color = color;
    this->shader = Shader::GIZMO;
}

void GizmoRenderer::init() {
    Component::init();
    Renderer::getActive()->registerRenderable(this, RenderPass::GIZMO);
}

void GizmoRenderer::render() {
    auto meshRenderer = getEntity()->getComponent<MeshRenderer>();
    if (meshRenderer) {
        scale = meshRenderer->getBoundingSphere().radius;
    }

    shader->bind();

    auto position = getEntity()->getTransform()->getWorldPosition();
    auto modelMatrix = glm::scale(glm::translate(glm::mat4(1), position), glm::vec3(scale));
    shader->setUniform("modelMatrix", modelMatrix);

    auto camera = Renderer::getActive()->getCamera();
    shader->setUniform("viewMatrix", camera->getView());
    shader->setUniform("projectionMatrix", camera->getProjection());

    shader->setUniform("color", color);

    glBindVertexArray(Mesh::SPHERE->vao);
    glDrawElements(GL_TRIANGLES, Mesh::SPHERE->indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void GizmoRenderer::remove() {
    Component::remove();
    Renderer::getActive()->removeRenderable(this, RenderPass::GIZMO);
}
