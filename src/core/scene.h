#pragma once

#include <vector>
#include "entity.h"
#include "window.h"
#include "../rendering/renderer.h"
#include "input.h"
#include "../physics/physicsWorld.h"

class Scene {
private:
    std::vector<Entity *> entities;
    Window *window;
    Renderer *renderer;
    Input *input;
    Entity *gui;
    PhysicsWorld *physicsWorld;
    btDiscreteDynamicsWorld *dynamicsWorld;

    static Scene *active;

    void addDefaultEntities();
    void initGui();
    void initSky();

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
