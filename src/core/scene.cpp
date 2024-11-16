#include "scene.h"
#include "window.h"
#include "../data/importer.h"
#include "../rendering/guiRenderer.h"
#include "../rendering/meshRenderer.h"

Scene *Scene::active = nullptr;

Scene::Scene() {
    window = new Window(800, 600, "OpenGL scene");
    renderer = new Renderer(window);
    input = new Input(window);
}

void Scene::load() {
}

void Scene::initGui() {
    auto gui = new Entity("gui");

    auto gui_rend = new GuiRenderer(window, input);
    gui->addComponent(gui_rend);

    spawn(gui);
}

void Scene::open() {
    active = this;

    window->open();
    input->init();
    initGui();

    Importer importer("../test.glb", input);
    importer.load();
    for (auto &entity: importer.getEntities()) {
        spawn(entity);
    }

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

    for (auto &entity: entities) {
        delete entity;
    }

    active = nullptr;
}

Scene *Scene::getActive() {
    return active;
}

Window *Scene::getWindow() {
    return window;
}

Renderer *Scene::getRenderer() {
    return renderer;
}

Input *Scene::getInput() {
    return input;
}

void Scene::spawn(Entity *entity) {
    entities.push_back(entity);
    entity->spawn();
}
