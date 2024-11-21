#include "scene.h"
#include "window.h"
#include "../data/importer.h"
#include "../rendering/guiRenderer.h"
#include "../rendering/meshRenderer.h"
#include "./time.h"
#include "physicsComponent.h"

Scene *Scene::active = nullptr;

Scene::Scene() {
    window = new Window(800, 600, "OpenGL scene");
    renderer = new Renderer(window);
    input = new Input(window);
    settings = new Settings();
    settings->subscribe(window);
}

void Scene::load() {
    physicsWorld = PhysicsWorld::getInstance();
    dynamicsWorld = physicsWorld->getDynamicsWorld();
}

void Scene::initGui() {
    gui = new Entity("gui");

    const auto gui_rend = new GuiRenderer(window, input, settings);
    gui->addComponent(gui_rend);

    spawn(gui);
}

void Scene::open() {
    active = this;

    window->open();
    input->init();
    renderer->init();
    initGui();

    Importer importer("..\\resources\\test.gltf");
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

        Time::endFrame();
    }

    window->close();
}

Scene::~Scene() {
    delete window;
    delete renderer;
    delete input;
    delete gui;

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
