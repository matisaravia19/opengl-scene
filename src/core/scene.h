#pragma once

#include <vector>
#include "entity.h"
#include "window.h"
#include "../rendering/renderer.h"
#include "input.h"

class Scene {
private:
    std::vector<Entity *> entities;
    Window *window;
    Renderer *renderer;
    Input *input;

    static Scene *active;

    void initGui();

public:
    Scene();
    ~Scene();

    void open();
    void load();

    void spawn(Entity *entity);

    static Scene *getActive();
    Window *getWindow();
    Renderer *getRenderer();
    Input *getInput();
};
