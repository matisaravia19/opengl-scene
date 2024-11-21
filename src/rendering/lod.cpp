#include "lod.h"
#include "../core/entity.h"

void LOD::init() {
    Component::init();

    renderer = getEntity()->getComponent<MeshRenderer>();

    for (auto &mesh: meshes) {
        mesh->upload();
    }
}

void LOD::update() {
    Component::update();

    auto camera = Renderer::getActive()->getCamera();
    auto distance = glm::distance(camera->getEntity()->getTransform()->getPosition(), getEntity()->getTransform()->getPosition());
    auto lod = 0;
    for (auto i = 0; i < meshes.size(); i++) {
        if (distance > distanceBetweenLODs * i) {
            lod = i;
        }
    }

    renderer->setMesh(meshes[lod]);
}

void LOD::addMesh(std::shared_ptr<Mesh> mesh) {
    meshes.push_back(mesh);
}

void LOD::setDistance(float distance) {
    distanceBetweenLODs = distance;
}
