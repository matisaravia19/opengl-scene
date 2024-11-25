#include "renderer.h"

#include "guiRenderer.h"
#include "../core/scene.h"
#include "shader.h"
#include "mesh.h"
#include "light.h"

Renderer::Renderer(Window *window) {
    this->window = window;
}

void Renderer::init() {
    uploadPrimitiveMeshes();
    uploadStandardShaders();
    initZBuffer(window->getWidth(), window->getHeight());
    initGBuffer(window->getWidth(), window->getHeight());
    initPPBuffer(window->getWidth(), window->getHeight());
}

void Renderer::render() {
    renderGBuffer();
    renderLighting();
    renderForward();
    renderGizmos();
    renderPostProcessing();

    glDisable(GL_DEPTH_TEST);

    for (Renderable *renderable: guiRenderables) {
        renderable->render();
    }

    window->swapBuffers();
}

void Renderer::renderGBuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    glDisable(GL_BLEND);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (Renderable *renderable: deferredRenderables) {
        renderable->render();
    }
}

void Renderer::renderLighting() {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    for (Light *light: lights) {
        light->renderShadow(deferredRenderables);
    }

    glDisable(GL_CULL_FACE);

    glBindFramebuffer(GL_FRAMEBUFFER, ppBuffer);
    glViewport(0, 0, window->getWidth(), window->getHeight());

    glDisable(GL_DEPTH_TEST);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gPosition);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gNormal);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, gMetallicRoughness);

    for (Light *light: lights) {
        light->renderDeferred();
    }
}

void Renderer::renderForward() {
    glBindFramebuffer(GL_FRAMEBUFFER, ppBuffer);

    glEnable(GL_DEPTH_TEST);

    for (Renderable *renderable: forwardRenderables) {
        renderable->render();
    }
}

void Renderer::renderGizmos() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDisable(GL_DEPTH_TEST);

    for (Renderable *renderable: gizmoRenderables) {
        renderable->render();
    }
}

void Renderer::renderPostProcessing() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    Shader::HDR->bind();
    Shader::HDR->setUniform("windowSize", window->getSize());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ppColor);

    drawFrameQuad();
}

void Renderer::registerRenderable(Renderable *renderable, RenderPass renderPass) {
    switch (renderPass) {
        case RenderPass::DEFERRED:
            deferredRenderables.insert(renderable);
            break;
        case RenderPass::FORWARD:
            forwardRenderables.insert(renderable);
            break;
        case RenderPass::GIZMO:
            gizmoRenderables.insert(renderable);
            break;
        case RenderPass::GUI:
            guiRenderables.insert(renderable);
            break;
    }
}

void Renderer::removeRenderable(Renderable *renderable, RenderPass renderPass) {
    switch (renderPass) {
        case RenderPass::DEFERRED:
            deferredRenderables.erase(renderable);
            break;
        case RenderPass::FORWARD:
            forwardRenderables.erase(renderable);
            break;
        case RenderPass::GIZMO:
            gizmoRenderables.erase(renderable);
            break;
        case RenderPass::GUI:
            guiRenderables.erase(renderable);
            break;
    }
}

void Renderer::setCamera(Camera *camera) {
    this->camera = camera;
}

Camera *Renderer::getCamera() {
    return camera;
}

Window *Renderer::getWindow() {
    return window;
}

Renderer *Renderer::getActive() {
    return Scene::getActive()->getRenderer();
}

void Renderer::registerLight(Light *light) {
    lights.insert(light);
}

void Renderer::removeLight(Light *light) {
    lights.erase(light);
}

void Renderer::onWindowResize(int width, int height) {
    glViewport(0, 0, width, height);

    resizeZBuffer(width, height);
    resizeGBuffer(width, height);
    resizePPBuffer(width, height);
}

void Renderer::uploadPrimitiveMeshes() {
    Mesh::QUAD->upload();
    Mesh::CUBE->upload();
}

void Renderer::uploadStandardShaders() {
    Shader::DEFERRED_POINT_LIGHT->upload();
    Shader::DEFERRED_DIRECTIONAL_LIGHT->upload();
    Shader::DEFERRED_SPOT_LIGHT->upload();
    Shader::SHADOW->upload();
    Shader::PBR->upload();
    Shader::GIZMO->upload();
    Shader::HDR->upload();
}

void Renderer::drawFrameQuad() {
    glBindVertexArray(Mesh::QUAD->vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Renderer::initGBuffer(int width, int height) {
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

    glGenTextures(1, &gAlbedo);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gAlbedo, 0);

    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gNormal, 0);

    glGenTextures(1, &gMetallicRoughness);
    glBindTexture(GL_TEXTURE_2D, gMetallicRoughness);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gMetallicRoughness, 0);

    unsigned int attachments[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
                                   GL_COLOR_ATTACHMENT3};
    glDrawBuffers(4, attachments);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, zBuffer, 0);
}

void Renderer::resizeGBuffer(int width, int height) {
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);

    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);

    glBindTexture(GL_TEXTURE_2D, gMetallicRoughness);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
}

void Renderer::initZBuffer(int width, int height) {
    glGenTextures(1, &zBuffer);
    glBindTexture(GL_TEXTURE_2D, zBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, zBuffer, 0);
}

void Renderer::resizeZBuffer(int width, int height) {
    glBindTexture(GL_TEXTURE_2D, zBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
}

void Renderer::initPPBuffer(int width, int height) {
    glGenFramebuffers(1, &ppBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, ppBuffer);

    glGenTextures(1, &ppColor);
    glBindTexture(GL_TEXTURE_2D, ppColor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ppColor, 0);

    unsigned int attachments[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, attachments);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, zBuffer, 0);
}

void Renderer::resizePPBuffer(int width, int height) {
    glBindTexture(GL_TEXTURE_2D, ppColor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
}
