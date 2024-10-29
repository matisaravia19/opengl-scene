#include "renderer.h"

Renderer::Renderer(Window *window) {
    this->window = window;
}

void Renderer::render() {
    glClearColor(0.3f, 0.7f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (Renderable *renderable : renderables) {
        renderable->render();
    }

    window->swapBuffers();
}

void Renderer::registerRenderable(Renderable *renderable) {
    renderables.insert(renderable);
}
