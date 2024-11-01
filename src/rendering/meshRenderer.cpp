#include "meshRenderer.h"

#include "../core/entity.h"

MeshRenderer::MeshRenderer(Mesh *mesh) {
    this->mesh = mesh;
    shader = new Shader("C:\\Users\\matis\\OneDrive\\Documentos\\Fing\\CGA\\opengl-scene\\src\\shaders\\test.vert",
                        "C:\\Users\\matis\\OneDrive\\Documentos\\Fing\\CGA\\opengl-scene\\src\\shaders\\test.frag");
}

void MeshRenderer::render() {
    shader->use();
    glBindVertexArray(mesh->vao);

    auto camera = Renderer::getActive()->getCamera();
    shader->setUniform("model", transform->getModelMatrix());
    shader->setUniform("view", camera->getView());
    shader->setUniform("projection", camera->getProjection());

    glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(0);
    shader->disable();
}

void MeshRenderer::init() {
    transform = getEntity()->getTransform();

    Renderer::getActive()->registerRenderable(this);
    mesh->upload();
}

void MeshRenderer::remove() {
    Renderer::getActive()->removeRenderable(this);
}
