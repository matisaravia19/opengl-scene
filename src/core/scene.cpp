#include "scene.h"
#include "window.h"

void Scene::load() {
    auto testEntity = Entity();
    testEntity.addComponent(new DebugComponent());
    this->entities.push_back(testEntity);
}

void Scene::open() {
    window = new Window(800, 600, "OpenGL scene");
    window->open();

    renderer = new Renderer(window);
    input = new Input(window);

    while (!window->shouldClose()) {
        input->poll();

        for (auto& entity : this->entities) {
            entity.update();
        }

        renderer->render();
    }

    window->close();
}

Scene::~Scene() {
    delete window;
    delete renderer;
}
