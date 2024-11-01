#include "renderer.h"
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

    for (Renderable *renderable: renderables) {
        renderable->render();
    }

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
