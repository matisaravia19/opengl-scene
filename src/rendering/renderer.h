#pragma once


#include <unordered_set>
#include "../core/window.h"
#include "camera.h"

class Renderer;

class Renderable {
public:
    virtual ~Renderable() = default;

    virtual void render() = 0;
};

class Renderer {
private:
    Window *window;
    Camera *camera;
    std::unordered_set<Renderable *> renderables;

public:
    explicit Renderer(Window *window);

    void render();
    void registerRenderable(Renderable *renderable);
    void removeRenderable(Renderable *renderable);
    void setCamera(Camera *camera);

    static Renderer *getActive();
    Camera *getCamera();
};
