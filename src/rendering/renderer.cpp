#include "renderer.h"

#include <iostream>

#include "guiRenderer.h"
#include "../core/scene.h"

Renderer::Renderer(Window *window) {
    this->window = window;
}

void Renderer::render() {
    glClearColor(0.3f, 0.7f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);
    glDepthFunc(GL_LESS);

    GuiRenderer *gui = nullptr;
    for (Renderable *renderable: renderables) {
        if (dynamic_cast<GuiRenderer*>(renderable)) { // force gui to render last
            gui = dynamic_cast<GuiRenderer*>(renderable);
            continue;
        }
        renderable->render();
    }
    if (gui) gui->render();

    window->swapBuffers();
}

void Renderer::registerRenderable(Renderable *renderable) {
    renderables.insert(renderable);
}

void Renderer::removeRenderable(Renderable *renderable) {
    renderables.erase(renderable);
}

void Renderer::setCamera(Camera *camera) {
    this->camera = camera;
}

Camera *Renderer::getCamera() {
    return camera;
}

Renderer *Renderer::getActive() {
    return Scene::getActive()->getRenderer();
}
