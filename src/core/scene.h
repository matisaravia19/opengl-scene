#pragma once

#include <vector>
#include "entity.h"
#include "window.h"
#include "../rendering/renderer.h"
#include "input.h"
#include "physicsWorld.h"

class Scene {
private:
    std::vector<Entity *> entities;
    Window *window;
    Renderer *renderer;
    Input *input;
    Entity *gui;
    Settings *settings;
    PhysicsWorld *physicsWorld;
    btDiscreteDynamicsWorld *dynamicsWorld;

    static Scene *active;

    void initGui();

public:
    Scene();
    ~Scene();

    void open();
    void load();

    void spawn(Entity *entity);
    void updateWorld();

    static Scene *getActive();
    Window *getWindow();
    Renderer *getRenderer();
    Input *getInput();
};
