#include "treeRenderer.h"
#include "../core/entity.h"
#include "../core/constants.h"

TreeRenderer::TreeRenderer(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> baseMaterial,
                           std::shared_ptr<Mesh> billboardMesh, std::shared_ptr<Material> billboardMaterial)
        : MeshRenderer(mesh, baseMaterial) {
    this->billboardMesh = billboardMesh;
    this->billboardMaterial = billboardMaterial;
}

void TreeRenderer::init() {
    MeshRenderer::init();
    billboardMesh->upload();
    billboardMaterial->upload();
}

void TreeRenderer::update() {
    MeshRenderer::update();

    auto camera = Renderer::getActive()->getCamera();
    auto position = getEntity()->getTransform()->getWorldPosition();
    auto distance = glm::distance(camera->getPosition(), position);
    isInBillboardMode = distance > SettingsManager::getSettings().treeDistance;
}

void TreeRenderer::render() {
    if (!isInBillboardMode) {
        MeshRenderer::render();
        return;
    }

    auto camera = Renderer::getActive()->getCamera();
    if (!camera->getFrustum().intersects(getBoundingSphere())) {
        return;
    }

    auto viewMatrix = camera->getView();
    auto projectionMatrix = camera->getProjection();

    billboardMaterial->bind();
    auto shader = billboardMaterial->getShader();

    setUniforms(shader, projectionMatrix, viewMatrix);

    glBindVertexArray(billboardMesh->vao);
    glDrawElements(GL_TRIANGLES, billboardMesh->indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void TreeRenderer::renderShadow(const Light *light) {
    if (isInBillboardMode) {
        return;
    }

    MeshRenderer::renderShadow(light);
}
