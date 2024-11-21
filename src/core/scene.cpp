#include "scene.h"
#include "window.h"
#include "../data/importer.h"
#include "../rendering/guiRenderer.h"
#include "../rendering/meshRenderer.h"
#include "./time.h"
#include "dayTimer.h"
#include "physicsComponent.h"
#include "../rendering/skyLight.h"
#include "../rendering/skybox.h"

Scene *Scene::active = nullptr;

Scene::Scene() {
    window = new Window(800, 600, "OpenGL scene");
    renderer = new Renderer(window);
    input = new Input(window);
    Settings::ActiveSettings->subscribe(window);
}

void Scene::load() {
    physicsWorld = PhysicsWorld::getInstance();
    dynamicsWorld = physicsWorld->getDynamicsWorld();
}

void Scene::initGui() {
    gui = new Entity("gui");

    const auto gui_rend = new GuiRenderer(window, input);
    gui->addComponent(gui_rend);

    spawn(gui);
}

void Scene::initSky() {
    auto sky = new Entity("sky");

    sky->addComponent(new DayTimer());
    sky->addComponent(new SkyLight());

    std::string vs_path = "../resources/shaders/sky/skybox.vert";
    std::string fs_path = "../resources/shaders/sky/skybox.frag";
    sky->addComponent(new Skybox(vs_path, fs_path));

    spawn(sky);
}

void Scene::open() {
    active = this;

    window->open();
    input->init();
    initGui();
    initSky();

    Importer importer("..\\resources\\scene.gltf");
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
