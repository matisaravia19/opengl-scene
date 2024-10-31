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

public:
    void load();
    void open();

    ~Scene();
};
