#pragma once


#include <unordered_set>
#include "../core/window.h"
#include "renderer.h"

class Renderer;

class Renderable {
protected:
    Renderer* renderer;
public:
    virtual ~Renderable() = default;

    virtual void render() = 0;
    void setRenderer(Renderer* renderer);
};

class Renderer {
private:
    Window *window;
    std::unordered_set<Renderable*> renderables;

public:
    explicit Renderer(Window *window);
    void render();

    void registerRenderable(Renderable *renderable);
    void removeRenderable(Renderable *renderable);
};
