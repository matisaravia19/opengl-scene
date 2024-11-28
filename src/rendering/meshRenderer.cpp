#include "meshRenderer.h"

#include <utility>
#include "../core/entity.h"
#include "shader.h"
#include "light.h"

MeshRenderer::MeshRenderer(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) {
    this->mesh = std::move(mesh);
    this->material = std::move(material);
}

void MeshRenderer::setUniforms(Shader *shader, glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {
    auto modelMatrix = transform->getModelMatrix();
    shader->setUniform("modelMatrix", modelMatrix);

    auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelMatrix)));
    shader->setUniform("normalMatrix", normalMatrix);

    auto camera = Renderer::getActive()->getCamera();
    shader->setUniform("viewMatrix", viewMatrix);
    shader->setUniform("projectionMatrix", projectionMatrix);
    shader->setUniform("cameraPosition", camera->getEntity()->getTransform()->getPosition());

    if (armature) {
        auto bones = armature->getBoneMatrices();
        shader->setUniform("bones", bones.data(), bones.size());
    }
}

void MeshRenderer::render() {
    auto camera = Renderer::getActive()->getCamera();

    if (!camera->getFrustum().intersects(getBoundingSphere())) {
        return;
    }

    auto viewMatrix = camera->getView();
    auto projectionMatrix = camera->getProjection();

    material->bind();
    auto shader = material->getShader();

    setUniforms(shader, projectionMatrix, viewMatrix);

    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void MeshRenderer::renderShadow(const Light *light) {
    if (!light->getShadowFrustum().intersects(getBoundingSphere())) {
        return;
    }

    auto shader = Shader::SHADOW;
    shader->bind();

    setUniforms(shader, light->getProjectionMatrix(), light->getViewMatrix());

    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void MeshRenderer::init() {
    Component::init();

    transform = getEntity()->getTransform();

    auto parent = getEntity()->getParent();
    if (parent) {
        armature = parent->getComponent<Armature>();
    }

    auto materialShaderType = material->getShader()->getType();
    renderPass = materialShaderType == ShaderType::DEFERRED ? RenderPass::DEFERRED : RenderPass::FORWARD;

    Renderer::getActive()->registerRenderable(this, renderPass);
    mesh->upload();
    material->upload();
}

void MeshRenderer::remove() {
    Component::remove();

    Renderer::getActive()->removeRenderable(this, renderPass);
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

Sphere MeshRenderer::getBoundingSphere() {
    auto scale = transform->getWorldScale();
    float radius = mesh->radius * glm::max(scale.x, glm::max(scale.y, scale.z));

    return Sphere(transform->getWorldPosition(), radius);
}

std::shared_ptr<Material> MeshRenderer::getMaterial() {
    return material;
}

void MeshRenderer::setMaterial(std::shared_ptr<Material> material) {
    this->material = material;
    if (getLifeState() == ComponentLifeState::INITIALIZED) {
        material->upload();
    }
}
