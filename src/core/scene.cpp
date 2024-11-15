#include "scene.h"
#include "window.h"
#include "../data/importer.h"
#include "../rendering/meshRenderer.h"
#include "physicsComponent.h"

Scene *Scene::active = nullptr;

Scene::Scene() {
    window = new Window(800, 600, "OpenGL scene");
    renderer = new Renderer(window);
    input = new Input(window);
}

void Scene::load() {
    physicsWorld = PhysicsWorld::getInstance();
    dynamicsWorld = physicsWorld->getDynamicsWorld();
}

void Scene::open() {
    active = this;

    window->open();
    input->init();

    Importer importer("../test.glb", input);
    importer.load();
    for (auto &entity: importer.getEntities()) {
        spawn(entity);
    }

    while (!window->shouldClose()) {
        input->poll();

        updateWorld();
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

void Scene::updateWorld() {
    const double current_frame = glfwGetTime();
    static double last_frame = 0.0f;

    dynamicsWorld->stepSimulation(current_frame - last_frame, 10);
    last_frame = current_frame;
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
