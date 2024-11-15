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
    PhysicsWorld *physicsWorld;
    btDiscreteDynamicsWorld *dynamicsWorld;

    static Scene *active;

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
