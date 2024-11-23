#pragma once


#include <unordered_set>
#include "../core/window.h"
#include "camera.h"
#include "light.h"

enum class RenderPass {
    DEFERRED = 0,
    FORWARD = 1,
    GUI = 2,
};

class Renderable {
public:
    virtual ~Renderable() = default;

    virtual void render() = 0;
};

class Renderer {
private:
    Window *window;
    Camera *camera;
    std::unordered_set<Renderable *> deferredRenderables;
    std::unordered_set<Renderable *> forwardRenderables;
    std::unordered_set<Renderable *> guiRenderables;
    std::unordered_set<Light *> lights;

    struct {
        unsigned int vao;
        unsigned int vbo;
        unsigned int ebo;
    } frameQuad;

    void initFrameQuad();

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

    void drawFrameQuad();

    static Renderer *getActive();
    Camera *getCamera();
    Window *getWindow();
};
