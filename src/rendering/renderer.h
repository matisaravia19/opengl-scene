#pragma once


#include <unordered_set>
#include "../core/window.h"
#include "camera.h"

enum class RenderPass {
    DEFERRED = 0,
    FORWARD = 1,
    GIZMO = 2,
    GUI = 3,
};

class Renderable {
public:
    virtual ~Renderable() = default;

    virtual void render() = 0;

    virtual void renderShadow(glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {}
};

class Light;

class Renderer {
private:
    Window *window;
    Camera *camera;
    std::unordered_set<Renderable *> deferredRenderables;
    std::unordered_set<Renderable *> forwardRenderables;
    std::unordered_set<Renderable *> gizmoRenderables;
    std::unordered_set<Renderable *> guiRenderables;
    std::unordered_set<Light *> lights;

    void uploadPrimitiveMeshes();

    void uploadStandardShaders();

    unsigned int zBuffer;

    void initZBuffer(int width, int height);
    void resizeZBuffer(int width, int height);

    unsigned int gBuffer;
    unsigned int gPosition, gAlbedo, gNormal, gMetallicRoughness;

    void initGBuffer(int width, int height);
    void resizeGBuffer(int width, int height);

    unsigned int ppBuffer;
    unsigned int ppColor;

    void initPPBuffer(int width, int height);
    void resizePPBuffer(int width, int height);

    void renderGBuffer();
    void renderLighting();
    void renderForward();
    void renderGizmos();
    void renderPostProcessing();
public:
    explicit Renderer(Window *window);

    void init();
    void render();

    void registerRenderable(Renderable *renderable, RenderPass renderPass);
    void removeRenderable(Renderable *renderable, RenderPass renderPass);

    void registerLight(Light *light);
    void removeLight(Light *light);

    void setCamera(Camera *camera);
    void onWindowResize(int width, int height);

    static void drawFrameQuad();

    static Renderer *getActive();
    Camera *getCamera();
    Window *getWindow();
};
