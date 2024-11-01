#include "scene.h"
#include "window.h"
#include "../data/importer.h"
#include "../rendering/meshRenderer.h"

Scene::Scene() {
    window = new Window(800, 600, "OpenGL scene");
    renderer = new Renderer(window);
    input = new Input(window);
}

void Scene::load() {
    auto testEntity = new Entity();
    testEntity->addComponent(new DebugComponent());
    auto meshRenderer = new MeshRenderer(Mesh());
    meshRenderer->setRenderer(renderer);
    testEntity.addComponent(meshRenderer);
    this->entities.push_back(testEntity);
}

void Scene::open() {
    window->open();

    for (auto& entity : this->entities) {
        entity.init();
    }

    Importer importer("C:\\Users\\matis\\OneDrive\\Documentos\\Fing\\CGA\\opengl-scene\\test.glb");
    importer.import();

    while (!window->shouldClose()) {
        input->poll();

        for (auto &entity: this->entities) {
            entity->update();
        }

        renderer->render();
    }

    window->close();
}

Scene::~Scene() {
    delete window;
    delete renderer;
}
