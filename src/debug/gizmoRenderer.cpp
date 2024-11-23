#include "gizmoRenderer.h"

#include "../core/entity.h"

static std::unique_ptr<Mesh> createCube() {
    std::vector<glm::vec3> vertices = {
            glm::vec3(-0.5f, -0.5f, -0.5f),
            glm::vec3(0.5f, -0.5f, -0.5f),
            glm::vec3(0.5f, 0.5f, -0.5f),
            glm::vec3(-0.5f, 0.5f, -0.5f),
            glm::vec3(-0.5f, -0.5f, 0.5f),
            glm::vec3(0.5f, -0.5f, 0.5f),
            glm::vec3(0.5f, 0.5f, 0.5f),
            glm::vec3(-0.5f, 0.5f, 0.5f)
    };

    std::vector<unsigned int> indices = {
            0, 1, 2, 2, 3, 0,
            1, 5, 6, 6, 2, 1,
            7, 6, 5, 5, 4, 7,
            4, 0, 3, 3, 7, 4,
            3, 2, 6, 6, 7, 3,
            4, 5, 1, 1, 0, 4
    };

    auto mesh = new Mesh("Gizmo cube", vertices.size(), indices.size());
    for (auto position: vertices) {
        Vertex vertex;
        vertex.position = position;
        mesh->vertices.push_back(vertex);
    }

    mesh->indices = indices;

    return std::unique_ptr<Mesh>(mesh);
}

std::unique_ptr<Mesh> GizmoRenderer::GIZMO_MESH = createCube();

GizmoRenderer::GizmoRenderer(glm::vec4 color) {
    this->color = color;
    this->shader = Shader::GIZMO;
}

void GizmoRenderer::init() {
    Component::init();
    Renderer::getActive()->registerRenderable(this, RenderPass::GIZMO);

    shader->upload();
    GIZMO_MESH->upload();
}

void GizmoRenderer::render() {
    shader->bind();

    auto position = getEntity()->getTransform()->getWorldPosition();
    auto modelMatrix = glm::scale(glm::translate(glm::mat4(1), position), glm::vec3(scale));
    shader->setUniform("modelMatrix", modelMatrix);

    auto camera = Renderer::getActive()->getCamera();
    shader->setUniform("viewMatrix", camera->getView());
    shader->setUniform("projectionMatrix", camera->getProjection());

    shader->setUniform("color", color);

    glBindVertexArray(GIZMO_MESH->vao);
    glDrawElements(GL_TRIANGLES, GIZMO_MESH->indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void GizmoRenderer::remove() {
    Component::remove();
    Renderer::getActive()->removeRenderable(this, RenderPass::GIZMO);
}
